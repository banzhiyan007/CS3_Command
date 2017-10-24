#ifndef BARCODE_93_HPP
#define BARCODE_93_HPP

class Barcode93: public Block
{
private :
    const char *Code93_Value;
    const char *CODE93[49];
    int  _dotsPerLineMax;
    char *_pLineBuf;
    int  _len;
	void init(void)
	{
		Code93_Value="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%&`()";
		CODE93=
		{
			"100010100",	//0
			"101001000",	//1
			"101000100",	//2
			"101000010",	//3
			"100101000",	//4
			"100100100",	//5
			"100100010",	//6
			"101010000",	//7
			"100010010",	//8
			"100001010",	//9
			"110101000",	//A
			"110100100",	//B
			"110100010",	//C
			"110010100",	//D
			"110010010",	//E
			"110001010",	//F
			"101101000",	//G
			"101100100",	//H
			"101100010",	//I
			"100110100",	//J
			"100011010",	//K
			"101011000",	//L
			"101001100",	//M
			"101000110",	//N
			"100101100",	//O
			"100010110",	//P
			"110110100",	//Q
			"110110010",	//R
			"110101100",	//S
			"110100110",	//T
			"110010110",	//U
			"110011010",	//V
			"101101100",	//W
			"101100110",	//X
			"100110110",	//Y
			"100111010",	//Z
			"100101110",	//-
			"111010100",	//.
			"111010010",	//SPACE
			"111001010",	//$
			"101101110",	//'/'
			"101110110",	//+
			"110101110",	//%
			"100100110",    //&
			"111011010",    //`
			"111010110",    //(   (/)
			"100110010",    //)	  (+)
			"101011110",	//*
			"000000000"     //NULL
		};
		_dotsPerLineMax = 1024;
	}
public :
    Barcode93(char *pdata,char *dispStr)
    {
		init();
        int i,j;
        char c,c1;
        char checkbuf[48+4];
        int bit_len;
        need_free = false;
        _pLineBuf = (char *)malloc(1024);
        memset(_pLineBuf,0,1024);
        char *disp_str = (char *)malloc(1024);
        memset(disp_str,0,1024);
        _len = strlen(pdata);
        if(_len > 60 ) {disp_str[0]=0;	goto err;}
        for(i=0;i<_len;i++)
        {
            if(!barcode_93_char_check(pdata[i])) goto err;
        }

        memcpy(disp_str,pdata,_len);disp_str[_len]=0;
        _pLineBuf[0]=0;
        barcode_93_addchar(_pLineBuf,'*',_dotsPerLineMax);//start
        for(i=0,j=0;i<_len;i++)
        {
            if (pdata[i]>127) {disp_str[0]=0;	goto err;}
            c=pdata[i];//CODE93_EXTENDED_ASCII[(int)(str[i])][0];
            if (!barcode_93_addchar(_pLineBuf,c,_dotsPerLineMax)) goto err;
            checkbuf[j++]=c;
        }
        c=code93_check(checkbuf,j,20);//C
        if (!barcode_93_addchar(_pLineBuf,c,_dotsPerLineMax)) goto err;
        checkbuf[j++]=c;
        c1=code93_check(checkbuf,j,15);//K
        if (!barcode_93_addchar(_pLineBuf,c1,_dotsPerLineMax)) goto err;
        if (!barcode_93_addchar(_pLineBuf,'*',_dotsPerLineMax)) goto err;//stop
        if((int)(strlen(_pLineBuf)+1)>_dotsPerLineMax) goto err;
        strcat(_pLineBuf,"1");//Termination Bar

        bit_len = strlen(_pLineBuf);
        for(i=0;i<bit_len;i++)
        {
            if(_pLineBuf[i]=='1')_pLineBuf[i/8]|=1<<(i%8);
            else _pLineBuf[i/8]&=~(1<<(i%8));
        }
        for(i=bit_len;i<bit_len+8;i++)
        {
            _pLineBuf[i/8]&=~(1<<(i%8));
        }
        Width = bit_len;
        Height = 1;
        pData = (uint8_t *)_pLineBuf;
        need_free = true;
        if(dispStr)strcpy(dispStr,disp_str);
        free(disp_str);
        return;
    err:
        pData = (uint8_t *)_pLineBuf;
        need_free = true;
        memset(_pLineBuf,0,1024);
        Width = 1;
        Height = 1;
        free(disp_str);
    }



private:

    int barcode_93_char_index(char ch)
    {
        int index;
        if    (ch>='0'&&ch<='9')index=ch-48;
        else if(ch>='A'&&ch<='Z')index=ch-55;
        else if(ch=='-')index=36;
        else if(ch=='.')index=37;
        else if(ch==' ')index=38;
        else if(ch=='$')index=39;
        else if(ch=='/')index=40;
        else if(ch=='+')index=41;
        else if(ch=='%')index=42;
        else if(ch=='&')index=43;
        else if(ch=='`')index=44;
        else if(ch=='(')index=45;
        else if(ch==')')index=46;
        else if(ch=='*')index=47;
        else index=48;
        return index;
    }
    //确认 ch 是否是 code39 有效字符
    bool barcode_93_char_check(char ch)
    {
        bool r = false;
        if    ((ch>='0')&&(ch<='9'))r = true;
        else if((ch>='A')&&(ch<='Z'))r = true;
         else if(ch=='-')r = true;
        else if(ch=='.')r = true;
        else if(ch==' ')r = true;
        else if(ch=='$')r = true;
        else if(ch=='/')r = true;
        else if(ch=='+')r = true;
        else if(ch=='%')r = true;
        else if(ch=='&')r = true;
        else if(ch=='`')r = true;
        else if(ch=='(')r = true;
        else if(ch==')')r = true;
        else if(ch=='*')r = true;
        else r = false;
        return r;
    }

    char code93_check(char *str,int str_len,int max)
    {
        int i,j=1;
        int checksum = 0;
        int index=0;
        for(i = str_len-1;i>=0;i--)
        {
            index=barcode_93_char_index(str[i]);
            checksum += index * (j++);
            if (j > max) j = 1;
        }
        checksum = checksum%47;
        return Code93_Value[checksum];
    }
    int barcode_93_addchar(char *dot_buf,char ch,int max_width)
    {
        int index = barcode_93_char_index(ch);
        if((int)(strlen(dot_buf)+strlen(CODE93[index]))<max_width)
        {
            strcat(dot_buf,CODE93[index]);
            return 1;
        }
        return 0;
    }



};

#endif
