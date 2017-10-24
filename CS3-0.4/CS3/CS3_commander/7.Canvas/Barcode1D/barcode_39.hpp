#ifndef BARCODE_39_HPP
#define BARCODE_39_HPP

class Barcode39 : public Block
{
private :
    const char *CODE39[45];
    const char *CODE39_EXTENDED_ASCII[128];
    int _DotsPerLineMax;
    char *_plineBuf;
	void init(void)
	{
		_DotsPerLineMax = 1024;
		CODE39=
		{
			"1010011011010",	//0
			"1101001010110",	//1
			"1011001010110",	//2
			"1101100101010",	//3
			"1010011010110",	//4
			"1101001101010",	//5
			"1011001101010",	//6
			"1010010110110",	//7
			"1101001011010",	//8
			"1011001011010",	//9
			"1101010010110",	//A	[10]
			"1011010010110",	//B
			"1101101001010",	//C
			"1010110010110",	//D
			"1101011001010",	//E
			"1011011001010",	//F
			"1010100110110",	//G
			"1101010011010",	//H
			"1011010011010",	//I
			"1010110011010",	//J
			"1101010100110",	//K, [20]
			"1011010100110",	//L
			"1101101010010",	//M
			"1010110100110",	//N
			"1101011010010",	//O
			"1011011010010",	//P
			"1010101100110",	//Q
			"1101010110010",	//R
			"1011010110010",	//S
			"1010110110010",	//T
			"1100101010110",	//U [30]
			"1001101010110",	//V
			"1100110101010",	//W
			"1001011010110",	//X
			"1100101101010",	//Y
			"1001101101010",	//Z
			"1001010110110",	//-
			"1100101011010",	//.
			"1001101011010",	//SPACE
			"1001001001010",	//$
			"1001001010010",	//'/' [40]
			"1001010010010",	//+
			"1010010010010",	//%
			"1001011011010",	//*
			"0000000000000"  	//NULL
		};
		CODE39_EXTENDED_ASCII= {
			"%U","$A","$B","$C","$D","$E","$F","$G",	// 0x7
			"$H","$I","$J","$K","$L","$M","$N","$O",	// 0xF
			"$P","$Q","$R","$S","$T","$U","$V","$W",	// 0x17
			"$X","$Y","$Z","%A","%B","%C","%D","%E",	// 0x1F
			" ", "/A","/B","/C","$","%","/F","/G", 	    // 0x27
			"/H","/I","*","+","/L","-", ".", "/",		// 0x2F
			"0", "1", "2", "3", "4", "5", "6", "7",		// 0x37
			"8", "9", "/Z","%F","%G","%H","%I","%J",	// 0x3F
			"%V","A", "B", "C", "D", "E", "F", "G",		// 0x47
			"H", "I", "J", "K", "L", "M", "N", "O",		// 0x4F
			"P", "Q", "R", "S", "T", "U", "V", "W",		// 0x57
			"X", "Y", "Z", "%K","%L","%M","%N","%O",	// 0x5F
			"%W","+A","+B","+C","+D","+E","+F","+G",	// 0x67
			"+H","+I","+J","+K","+L","+M","+N","+O",	// 0x6F
			"+P","+Q","+R","+S","+T","+U","+V","+W",	// 0x77
			"+X","+Y","+Z","%P","%Q","%R","%S","%T"		// 0x7F
		};
	}
public :
    Barcode39(char *pdata,char *dispStr)
    {
		init();
        int i;
        char c,c1;
        int bit_len;
        int strLen = strlen(pdata);
        _plineBuf = (char *)malloc(1024);
		memset(_plineBuf,0,1024);
        char *disp_str = (char *)malloc(256);
        memset(disp_str,0,256);

		if(pdata[0]=='*'){memcpy(disp_str,pdata+1,strLen-2);disp_str[strLen-2]=0;}
		else{memcpy(disp_str,pdata,strLen);disp_str[strLen]=0;}
        need_free = false;

        if(strLen > 60 ||strLen<1) goto err;

        _plineBuf[0]=0;
        barcode_39_addchar(_plineBuf,'*',_DotsPerLineMax);
        for(i=0;i<(int)strlen(disp_str);i++)
        {
            if (!barcode_39_char_check(disp_str[i]))goto err;
            c=CODE39_EXTENDED_ASCII[(int)(disp_str[i])][0];
            if (!barcode_39_addchar(_plineBuf,c,_DotsPerLineMax)) goto err ;

            c1=CODE39_EXTENDED_ASCII[(int)(disp_str[i])][1];
            if (c1!=0) if (!barcode_39_addchar(_plineBuf,c1,_DotsPerLineMax)) goto err ;
        }
        barcode_39_addchar(_plineBuf,'*',_DotsPerLineMax);

        bit_len = strlen(_plineBuf);
        for(i=0;i<bit_len;i++)
        {
            if(_plineBuf[i]=='1')_plineBuf[i/8]|=1<<(i%8);
            else _plineBuf[i/8]&=~(1<<(i%8));
        }
        for(i=bit_len;i<bit_len+8;i++)
        {
            _plineBuf[i/8]&=~(1<<(i%8));
        }
        Width = bit_len;
        Height = 1;
        pData = (uint8_t *)_plineBuf;
        need_free = true;
        if(dispStr)
		{
			strcpy(dispStr,"*");
			strcat(dispStr,disp_str);
			strcat(dispStr,"*");
		}
        free(disp_str);
        return ;
    err:
        need_free = true;
        memset(_plineBuf,0,1024);
        pData = (uint8_t *)_plineBuf;
        Width = 1;
        Height = 1;
        free(disp_str);
    }
private :
    int barcode_39_char_index(char ch)
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
        else if(ch=='*')index=43;
        else index=44;
        return index;
    }
    //确认 ch 是否是 code39 有效字符
    bool barcode_39_char_check(char ch)
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
        else if(ch=='*')r = true;
        else r = false;
        return r;
    }

    int barcode_39_addchar(char *dot_buf,char ch,int max_width)
    {
        int index = barcode_39_char_index(ch);
        if((int)(strlen(dot_buf)+strlen(CODE39[index]))>=max_width) return 0;
        strcat(dot_buf,CODE39[index]);
        return 1;
    }
    int barcode_make_CODE39_std(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int i;
        if(str_len > 60 || str_len<3 ) return 0 ;
        if (str[0]!='*' || str[str_len-1]!='*') return 0;
        memcpy(disp_str,str+1,str_len-2);disp_str[str_len-2]=0;
        line_buf[0]=0;
        for(i=0;i<str_len;i++)
        {
            if (!barcode_39_addchar(line_buf,str[i],line_max_len)) return 1;
        }
        return 1;
    }
    int barcode_make_CODE39_std_auto(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int i;
        if(str_len > 60 ||str_len<1) return 0;
        memcpy(disp_str,str,str_len);disp_str[str_len]=0;
        line_buf[0]=0;
        barcode_39_addchar(line_buf,'*',line_max_len);
        for(i=0;i<str_len;i++)
        {
            if (!barcode_39_addchar(line_buf,str[i],line_max_len)) return 1;
        }
        barcode_39_addchar(line_buf,'*',line_max_len);
        return 1;
    }
    int barcode_make_CODE39_std_auto_and_check(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int i;
        int index;
        int checksum;
        if(str_len > 60 ||str_len<1) return 0;
        memcpy(disp_str,str,str_len);disp_str[str_len]=0;
        line_buf[0]=0;
        barcode_39_addchar(line_buf,'*',line_max_len);//Start *
        checksum=0;
        for(i=0;i<str_len;i++)
        {
            if (!barcode_39_addchar(line_buf,str[i],line_max_len)) return 1;
            index = barcode_39_char_index(str[i]);
            if (index<43) {	checksum+=index;}
        }
        checksum=checksum%43;
        if((int)(strlen(line_buf)+strlen(CODE39[checksum]))<line_max_len)//增加校验和
        {
            strcat(line_buf,CODE39[checksum]);
        } else return 1;
        barcode_39_addchar(line_buf,'*',line_max_len);//Stop *
        return 1;
    }
};

#endif
