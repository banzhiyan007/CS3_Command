#ifndef UI_LCD_MESSAGE_H
#define UI_LCD_MESSAGE_H

class UiLcdMessage
{
private:
    uint32_t  MSG_MAX;
    uint32_t  MSGDISPTIME;
    uint32_t  INTERVALTIME;
    typedef struct
    {
        const char *pText;
        uint32_t    Timer;
        uint32_t    DisplayTime;
        bool        Active;
        bool        Used;
        bool        TmpMsg;
    }MSG;
	uint8_t     MsgCount;               // 注册消息个数
	MSG        *Msg_Array;         	    //注册消息数组
	uint8_t 	DipMsgCount;            //需要显示消息的个数
	MSG*       *Disp_Msg_Array;   	    //循环消息数组
	MSG        *pCur_Disp_Msg;          //当前指针，指向正要显示的消息
	uint8_t 	MsgNeedClear;
	uint8_t 	PreDipMsgCount;
	uint8_t 	circle_count;           // 循环计数器 每50ms计数一次
	uint8_t 	DispIndex;              // 循环队列消息序列数
	bool 	    IfChangeStatu;			//查找次数是超过一次
	uint32_t 	inittime;
	bool 		IsStatus1;
private:
    Lcd *lcd;
    Timer *timer;
    uint32_t timer_get(void)
    {
        return timer->read_ms();
    }
    uint32_t timer_lapse(uint32_t since_time)
    {
        uint32_t now = timer->read_ms();
        if(now>since_time)return (now-since_time);
        else return (0xFFFFFFFF-(since_time-now));
    }
    void output_clear(void)
    {
        lcd->printf16(0,16,"                ");
    }
    void output_msg(const char *pText)
    {
        output_clear();
        lcd->printf16((128-strlen(pText)*8)/2,16,pText);
    }
private:
    void  get_disp_msg_count(void)
    {
        uint32_t i;
        uint8_t LastMsgCount;
        LastMsgCount = DipMsgCount;
        DipMsgCount=0;
        for(i=0;i<MSG_MAX;i++)
        {
            if(Msg_Array[i].Active==true)
            {
                if((Msg_Array[i].DisplayTime) > 0 && (timer_lapse(Msg_Array[i].Timer) > (Msg_Array[i].DisplayTime)))
                {
                    off(&Msg_Array[i]);
                    Msg_Array[i].Active = false;
                    Msg_Array[i].Used = false;
                }
                Disp_Msg_Array[DipMsgCount++] =&Msg_Array[i];
            }
        }
        if(PreDipMsgCount != DipMsgCount){circle_count=0;IfChangeStatu=true;}
        PreDipMsgCount=DipMsgCount;

        if(DipMsgCount==0&&LastMsgCount!=0)MsgNeedClear=1;
    }
    void state_1_msg(void)
    {
        if(IsStatus1==true)
        {
            IsStatus1 = false;
            output_msg(Disp_Msg_Array[0]->pText);
        }
    }
    void state_mul_msg(void)
    {
        int i;
        if(IfChangeStatu==true)
        {
            for(i=0;i<DipMsgCount;i++)
            {
                if(Disp_Msg_Array[i] == pCur_Disp_Msg)
                {
                    DispIndex=i;
                    IfChangeStatu=false;
                    output_msg(Disp_Msg_Array[DispIndex]->pText);
                    inittime= timer_get();
                }
            }
        }
        if(timer_lapse(inittime) >= MSGDISPTIME)
        {
            output_clear();
        }
        if(timer_lapse(inittime) >= (MSGDISPTIME+INTERVALTIME))
        {
            DispIndex=(DispIndex+1)%DipMsgCount;
            output_msg(Disp_Msg_Array[DispIndex]->pText);
            inittime = timer_get();
        }
    }
    void state_no_msg(uint8_t CurMsgCount)
    {
        switch(CurMsgCount)
        {
        case(0):
            {
                if(MsgNeedClear)
                {
                    MsgNeedClear=0;
                    output_clear();
                }
            }
            break;
        case(1):{state_1_msg();}
            break;
        default:{state_mul_msg();}
            break;
        }
    }
public:
    UiLcdMessage(Lcd *_lcd)
    {
		MSG_MAX=16;
		MSGDISPTIME=1000;	//2000	消息切换显示时间:2000(ms)
		INTERVALTIME=250;   //250	消息切换间隔时间:250(ms)
        lcd = _lcd;
        Msg_Array = (MSG*)malloc(sizeof(MSG)*MSG_MAX);
        Disp_Msg_Array = (MSG**)malloc(sizeof(MSG *)*MSG_MAX);
        memset(Msg_Array,0,sizeof(MSG)*MSG_MAX);
        memset(Disp_Msg_Array,0,sizeof(MSG *)*MSG_MAX);
        DipMsgCount=0;              //需要显示消息的个数
        MsgNeedClear=0;
        PreDipMsgCount=0;
        MsgCount=0;                 // 注册消息个数
        circle_count=0;             // 循环计数器 每50ms计数一次
        DispIndex=0;				// 循环队列消息序列数
        IfChangeStatu=true;         //查找次数是超过一次
        inittime=0;
        IsStatus1=true;
        timer = new Timer;
        timer->start();
    }
    void status_refresh(bool redraw)
    {
        if(redraw)
        {
            MsgNeedClear=1;
            IsStatus1 = true;
            inittime = 0;//timer_get();
        }
        get_disp_msg_count();
        state_no_msg(DipMsgCount);
    }
    void *add(void)
    {
        uint32_t i;
        for(i=0;i<MSG_MAX;i++)
        {
            if(!Msg_Array[i].Used)
            {
                Msg_Array[i].Used = true;
                Msg_Array[i].TmpMsg = false;
                Msg_Array[i].pText = "";
                Msg_Array[i].Active = false;
                return &Msg_Array[i];
            }
        }
        return NULL;
    }
    void on(void *HandleMsg,const char *pText,uint32_t DisplayTime)
    {
        if(HandleMsg==NULL)return;
        MSG *pMsg = (MSG *)HandleMsg;
        pCur_Disp_Msg=pMsg;
        pMsg->pText = pText;
        pMsg->Timer = timer_get();
        pMsg->DisplayTime = DisplayTime;
        pMsg->Active = true;
        circle_count=0;
        IfChangeStatu=true;
        IsStatus1 = true;
    }
    void off(void *HandleMsg)
    {
        if(HandleMsg==NULL)return;
        MSG *pMsg = (MSG *)HandleMsg;
        pMsg->Active = false;
        DispIndex=0;
    }
    void display_message(const char *pText,uint32_t DisplayTime=0)
    {
        uint32_t i;
        for(i=0;i<MSG_MAX;i++)
        {
            if(Msg_Array[i].Used)
            if(Msg_Array[i].TmpMsg)
            if(strcmp(Msg_Array[i].pText,pText)==0)
            {
                Msg_Array[i].Active = true;
                Msg_Array[i].Timer = timer_get();
                Msg_Array[i].DisplayTime = DisplayTime;
                pCur_Disp_Msg=&Msg_Array[i];
                circle_count=0;
                IfChangeStatu=true;
                IsStatus1 = true;
                return;
            }
        }
        for(i=0;i<MSG_MAX;i++)
        {
            if(!Msg_Array[i].Used)
            {
                Msg_Array[i].Used = true;
                Msg_Array[i].TmpMsg = true;
                Msg_Array[i].pText = pText;
                Msg_Array[i].Active = true;
                Msg_Array[i].Timer = timer_get();
                Msg_Array[i].DisplayTime = DisplayTime;
                pCur_Disp_Msg=&Msg_Array[i];
                circle_count=0;
                IfChangeStatu=true;
                IsStatus1 = true;
                return;
            }
        }
    }
    void hide_message(const char *pText)
    {
        uint32_t i;
        for(i=0;i<MSG_MAX;i++)
        {
            if(Msg_Array[i].Used)
            if(Msg_Array[i].TmpMsg)
            if(strcmp(Msg_Array[i].pText,pText)==0)
            {
                Msg_Array[i].Used = false;
                Msg_Array[i].Active = false;
                DispIndex=0;
                return;
            }
        }
    }
    void show(const char *pText,bool enable,uint32_t DisplayTime=0)
    {
        if(enable)display_message(pText,DisplayTime);
        else hide_message(pText);
		status_refresh(true);
    }
};

#endif

