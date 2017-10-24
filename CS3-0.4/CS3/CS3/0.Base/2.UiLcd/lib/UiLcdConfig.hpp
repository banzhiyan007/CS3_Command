#ifndef UI_LCD_CONFIG_H
#define UI_LCD_CONFIG_H

#include <list>

class UiLcdConfig
{
public:
	enum CONFIG_FUNC_MODE
	{
		CONFIG_FUNC_ONCE,
		CONFIG_FUNC_REPEAT,
	};
	int EnterConfigMode;
	int ConfigMode;
public:
	Lcd    *_lcd;
	Button *btn_config;
	Button *btn_select;
	Button *btn_cancel;
	Button *btn_fn;
public:
	Icon icon_config;
	int CONFIG_TIMEOUT;
	Timer timer;
	enum CONFIG_TYPE
	{
		CONFIG_TYPE_INT,
		CONFIG_TYPE_FUNC,
		CONFIG_TYPE_INFO,
		CONFIG_TYPE_ENUM,
		CONFIG_TYPE_REG_U32,
		CONFIG_TYPE_QUICK_INFO,
	};
	struct CONFIG_ITEM
	{
		const char *Name;
		CONFIG_TYPE Type;
		uint32_t *pValue;
		uint32_t IntMin;
		uint32_t IntMax;
		const char *pValueList;
		FunctionPointer *pFuncSelected;
		FunctionPointer2Arg *pFuncDisplay;
		enum CONFIG_FUNC_MODE FuncMode;
	};
	list<CONFIG_ITEM> ConfigList[3];
	list<CONFIG_ITEM>::iterator ListItem[3];
	CONFIG_ITEM *pConfigItem[3];
	Button *BtnConfig[3];
	Button *BtnSelect[3];
	int string_enum_count(const char *str)
	{
		int n=1;
		char *ptr=(char*)str;
		while(ptr)
		{
			if((ptr = strchr(ptr,'|'))!=NULL){n++;ptr++;}
		}
		return n;
	}
	void string_enum_get_item(const char *str,int index,int *pValue,char Item[32+1])
	{
		int n=1;
		char *ptr=(char*)str;
		while(ptr)
		{
			if(index==n-1)
			{
				char *p1=strchr(ptr,':');
				if(p1==NULL){*pValue=0;if(Item)strcpy(Item,"None");return;}
				char buf[32];
				memset(buf,0,32);
				strncpy(buf,ptr,p1-ptr);
				*pValue=atol(buf);
				if(Item)
				{
					p1++;
					char *p2=strchr(p1,'|');
					memset(Item,0,32+1);
					if(p2==NULL)strncpy(Item,p1,32);
					else strncpy(Item,p1,p2-p1);
				}
				return;
			}
			if((ptr = strchr(ptr,'|'))!=NULL){n++;ptr++;}
		}
	}
	char *string_enum_get_item_name(int group,const char *str,int value)
	{
		static char ValueName[32+1];
		int i,n=string_enum_count(pConfigItem[group]->pValueList);
		for(i=0;i<n;i++)
		{
			int Value=0;
			string_enum_get_item(pConfigItem[group]->pValueList,i,&Value,ValueName);
			if(Value==value)return ValueName;
		}
		sprintf(ValueName,"%d",value);
		return ValueName;
	}
public:
	UiLcdConfig(Lcd *pLcd,Button *pBtnConfig,Button *pBtnSelect,Button *pBtnCancel,Button *pBtnFn)
	{
		icon_config={0,0,24,12,"\x30\x00\x00\xB6\x01\x00\xFE\x01\x00\xFC\x00\x00\xCF\x03\x00\xCF\xB3\x73\xFC\x88\x08\xFE\x89\x6B\xB6\x89\x48\x30\xB0\x70\x00\x00\x00\x00\x00\x00"};
		CONFIG_TIMEOUT=10000;
		EnterConfigMode=0;
		ConfigMode=0;
		_lcd = pLcd;
		btn_config = pBtnConfig;
		btn_select = pBtnSelect;
		btn_cancel = pBtnCancel;
		btn_fn     = pBtnFn;
		BtnConfig[0]=pBtnConfig;
		BtnSelect[0]=pBtnSelect;
		BtnConfig[1]=pBtnFn;
		BtnSelect[1]=pBtnSelect;
		BtnConfig[2]=pBtnSelect;
		BtnSelect[2]=pBtnConfig;
		timer.start();
	}
	void status_refresh(bool redraw)
	{
		static int last_config_mode=false;
		static Timer display_timer;
		if(btn_config->active()||btn_select->active()||btn_cancel->active()||btn_fn->active())sleep_clear();
		if(!ConfigMode)
		{
			if(EnterConfigMode==0)
			{
				if(btn_cancel->active())btn_config->down_time_clear();
				if(btn_config->down_time()>1000)
				{
					ConfigMode=1;
					btn_config->press_clear();
					timer.reset();
					return;
				}
			}
			else if(EnterConfigMode==1)
			{
				static int allow_enter_config=0;
				if(btn_cancel->active()&&!btn_config->active())allow_enter_config=1;
				if(!btn_cancel->active())allow_enter_config=0;

				if(allow_enter_config&&btn_cancel->active()&&btn_config->active())
				{
					ConfigMode=1;
					btn_config->press_clear();
					timer.reset();
					return;
				}
				if(allow_enter_config&&btn_cancel->active()&&btn_select->active())
				{
					ConfigMode=3;
					btn_select->press_clear();
					timer.reset();
					return;
				}
			}
			if(btn_fn->down_time()>1000)
			{
				ConfigMode=2;
				btn_fn->press_clear();
				timer.reset();
				return;
			}
		}
		else
		{
			int group=ConfigMode-1;
			if(EnterConfigMode==0)
			{
				if(btn_cancel->active()||(group==0&&btn_fn->active())||(group==1&&btn_config->active())||timer.read_ms()>CONFIG_TIMEOUT)
				{
					ConfigMode=0;
					btn_fn->down_time_clear();
					btn_config->down_time_clear();
					if(pConfigItem[group]->Type==CONFIG_TYPE_INFO)pConfigItem[group]->IntMax=0;
					return;
				}
			}
			else if(EnterConfigMode==1)
			{
				if((btn_cancel->pressed()&&timer.read_ms()>200)||(group==0&&btn_fn->active())||(group==1&&btn_config->active())||timer.read_ms()>CONFIG_TIMEOUT)
				{
					ConfigMode=0;
					btn_fn->down_time_clear();
					btn_config->down_time_clear();
					if(pConfigItem[group]->Type==CONFIG_TYPE_INFO)pConfigItem[group]->IntMax=0;
					return;
				}
			}

			if(pConfigItem[group]->Type==CONFIG_TYPE_INFO)
			{
				if(pConfigItem[group]->IntMax!=0)
				{
					if(btn_select->pressed())
					{
						pConfigItem[group]->IntMin++;
						display_timer.reset();
						timer.reset();
						_lcd->clear();
						pConfigItem[group]->pFuncDisplay->call((uint8_t*)&pConfigItem[group]->IntMin,pConfigItem[group]->IntMin);
						redraw = true;
					}
					if(display_timer.read_ms()>=50)
					{
						display_timer.reset();
						timer.reset();
						_lcd->clear();
						pConfigItem[group]->pFuncDisplay->call((uint8_t*)&pConfigItem[group]->IntMin,pConfigItem[group]->IntMin);
						redraw = true;
					}
					if(BtnConfig[group]->pressed())
					{
						pConfigItem[group]->IntMax=0;
						_lcd->clear();
						redraw = true;
						last_config_mode = 0;
						goto next1;
					}
					return;
				}
			}
next1:
			if(last_config_mode!=ConfigMode)
			{
				_lcd->clear();
				if(ConfigMode==1)_lcd->display_icon(&icon_config);
				if(ConfigMode==2)_lcd->printf12(0,0,"FN:");
				if(ConfigMode==3)_lcd->clear();
			}
			if(BtnConfig[group]->active()||btn_select->active())timer.reset();
		}
		last_config_mode=ConfigMode;
		if(ConfigMode)
		{
			int group=ConfigMode-1;
			if(BtnConfig[group]->pressed()&&!btn_cancel->active())
			{
				ListItem[group]++;
				if(ListItem[group]==ConfigList[group].end())ListItem[group]=ConfigList[group].begin();
				pConfigItem[group] = &(*ListItem[group]);
				redraw = true;
			}
			if(BtnSelect[group]->pressed())
			{
				if(pConfigItem[group]->Type==CONFIG_TYPE_INT)
				{
					if(++(*(pConfigItem[group]->pValue))>pConfigItem[group]->IntMax){*(pConfigItem[group]->pValue)=pConfigItem[group]->IntMin;}
					pConfigItem[group]->pFuncSelected->call();
					redraw = true;
				}
				if(pConfigItem[group]->Type==CONFIG_TYPE_FUNC)
				{
					if(pConfigItem[group]->FuncMode==CONFIG_FUNC_ONCE)
					{
						pConfigItem[group]->pFuncSelected->call();
						ConfigMode=0;
						return;
					}
					if(pConfigItem[group]->FuncMode==CONFIG_FUNC_REPEAT)
					{
						_lcd->puts16(0,16,"                ");
						_lcd->update();
						wait_ms(100);
						pConfigItem[group]->pFuncSelected->call();
						redraw = true;
					}
				}
				if(pConfigItem[group]->Type==CONFIG_TYPE_INFO)
				{
					pConfigItem[group]->IntMax = 1;
					display_timer.reset();
					timer.reset();
					_lcd->clear();
					pConfigItem[group]->pFuncDisplay->call((uint8_t*)&pConfigItem[group]->IntMin,pConfigItem[group]->IntMin);
					redraw = true;
					return;
				}
				if(pConfigItem[group]->Type==CONFIG_TYPE_ENUM)
				{
					int i,n=string_enum_count(pConfigItem[group]->pValueList);
					int Value=0;
					for(i=0;i<n;i++)
					{
						string_enum_get_item(pConfigItem[group]->pValueList,i,&Value,NULL);
						if(*(pConfigItem[group]->pValue)==(uint32_t)Value)break;
					}
					i=(i+1)%n;
					string_enum_get_item(pConfigItem[group]->pValueList,i,&Value,NULL);
					*(pConfigItem[group]->pValue)=Value;
					pConfigItem[group]->pFuncSelected->call();
					redraw = true;
				}
				if(pConfigItem[group]->Type==CONFIG_TYPE_REG_U32)
				{
					int i,n=string_enum_count(pConfigItem[group]->pValueList);
					int Value=0;
					for(i=0;i<n;i++)
					{
						string_enum_get_item(pConfigItem[group]->pValueList,i,&Value,NULL);
						if(*(pConfigItem[group]->pValue)==(uint32_t)Value)break;
					}
					i=(i+1)%n;
					string_enum_get_item(pConfigItem[group]->pValueList,i,&Value,NULL);
					*(pConfigItem[group]->pValue)=Value;
					if(pConfigItem[group]->pFuncSelected)pConfigItem[group]->pFuncSelected->call();
					reg_value_save();
					redraw = true;
				}
			}
			if(redraw)
			{	
				static int need_clear=0;
				if(need_clear){need_clear=0;_lcd->clear();}
				_lcd->puts16(0,16,"                ");
				if(pConfigItem[group]->Type==CONFIG_TYPE_INT)_lcd->printf16(0,16,"%s:%d",pConfigItem[group]->Name,*(pConfigItem[group]->pValue));
				if(pConfigItem[group]->Type==CONFIG_TYPE_FUNC)_lcd->printf16(0,16,"%s",pConfigItem[group]->Name);
				if(pConfigItem[group]->Type==CONFIG_TYPE_INFO)_lcd->printf16(0,16,"%s",pConfigItem[group]->Name);
				if(pConfigItem[group]->Type==CONFIG_TYPE_ENUM)_lcd->printf16(0,16,"%s:%s",pConfigItem[group]->Name,string_enum_get_item_name(group,pConfigItem[group]->pValueList,*(pConfigItem[group]->pValue)));
				if(pConfigItem[group]->Type==CONFIG_TYPE_REG_U32)_lcd->printf16(0,16,"%s:%s",pConfigItem[group]->Name,string_enum_get_item_name(group,pConfigItem[group]->pValueList,*(pConfigItem[group]->pValue)));
				if(pConfigItem[group]->Type==CONFIG_TYPE_QUICK_INFO)
				{
					_lcd->clear();
					pConfigItem[group]->pFuncDisplay->call((uint8_t*)&pConfigItem[group]->IntMin,pConfigItem[group]->IntMin);
					redraw = true;
					need_clear=1;
				}
			}
		}
	}
	void add_integer(int group,const char *name,uint32_t *pValue,uint32_t min,uint32_t max,FunctionPointer *func)
	{
		CONFIG_ITEM item;
		item.Name = name;
		item.Type = CONFIG_TYPE_INT,
		item.pValue = pValue;
		item.IntMin = min;
		item.IntMax = max;
		item.pFuncSelected = func;
		ConfigList[group].push_front(item);
		ListItem[group] = ConfigList[group].begin();
		pConfigItem[group] = &(*ListItem[group]);
	}
	void add_enum(int group,const char *name,uint32_t *pValue,const char *pValueList,FunctionPointer *func)
	{
		CONFIG_ITEM item;
		item.Name = name;
		item.Type = CONFIG_TYPE_ENUM,
		item.pValue = pValue;
		item.pValueList = pValueList;
		item.pFuncSelected = func;
		ConfigList[group].push_front(item);
		ListItem[group] = ConfigList[group].begin();
		pConfigItem[group] = &(*ListItem[group]);
	}
	void add_function(int group,const char *name,FunctionPointer *func,CONFIG_FUNC_MODE mode=CONFIG_FUNC_REPEAT)
	{
		CONFIG_ITEM item;
		item.Name = name;
		item.Type = CONFIG_TYPE_FUNC,
		item.FuncMode = mode;
		item.pFuncSelected = func;
		ConfigList[group].push_front(item);
		ListItem[group] = ConfigList[group].begin();
		pConfigItem[group] = &(*ListItem[group]);
	}
	void add_display_info(int group,const char *name,FunctionPointer2Arg *func)
	{
		CONFIG_ITEM item;
		item.Name = name;
		item.Type = CONFIG_TYPE_INFO,
		item.IntMin = 0;
		item.IntMax = 0;
		item.pFuncDisplay = func;
		ConfigList[group].push_front(item);
		ListItem[group] = ConfigList[group].begin();
		pConfigItem[group] = &(*ListItem[group]);
	}
	void add_quick_info(int group,const char *name,FunctionPointer2Arg *func)
	{
		CONFIG_ITEM item;
		item.Name = name;
		item.Type = CONFIG_TYPE_QUICK_INFO,
		item.IntMin = 0;
		item.IntMax = 0;
		item.pFuncDisplay = func;
		ConfigList[group].push_front(item);
		ListItem[group] = ConfigList[group].begin();
		pConfigItem[group] = &(*ListItem[group]);
	}
	void add_reg_u32(int group,const char *name,const char *reg_name,FunctionPointer *func=NULL)
	{
		int index = reg_index(reg_name);
		CONFIG_ITEM item;
		item.Name = name;
		item.Type = CONFIG_TYPE_REG_U32,
		item.pValue = (uint32_t*)reg_get_data_ptr(index);
		item.pValueList = reg_info(index);
		item.pFuncSelected = func;
		ConfigList[group].push_front(item);
		ListItem[group] = ConfigList[group].begin();
		pConfigItem[group] = &(*ListItem[group]);
	}
};

#endif

