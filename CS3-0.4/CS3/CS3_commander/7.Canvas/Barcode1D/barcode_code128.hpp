#ifndef BARCODE_CODE128_H
#define BARCODE_CODE128_H
#include "mbed.h"

class BarcodeCode128 : public Block
{
private :
    int CODE128_FORMAT_A;
    int CODE128_FORMAT_B;
    int CODE128_FORMAT_C;
    int V_CODE_A;
    int V_CODE_B;
    int V_CODE_C;
    int V_START_A;
    int V_START_B;
    int V_START_C;

    const char *Code128Rule[106];
	int line_max_len;
    unsigned short disp_code[48+4];

    int str_len;
    char *line_buf;
    char *disp_str;
    int bit_len;
	void init(void)
	{
		CODE128_FORMAT_A = 1;
		CODE128_FORMAT_B = 2;
		CODE128_FORMAT_C = 3;
		V_CODE_A = 101;
		V_CODE_B = 100;
		V_CODE_C = 99;
		V_START_A = 103;
		V_START_B = 104;
		V_START_C = 105;

		Code128Rule=
		{
			"11011001100",
			"11001101100",
			"11001100110",
			"10010011000",
			"10010001100",
			"10001001100",
			"10011001000",
			"10011000100",
			"10001100100",
			"11001001000",
			"11001000100",
			"11000100100",
			"10110011100",
			"10011011100",
			"10011001110",
			"10111001100",
			"10011101100",
			"10011100110",
			"11001110010",
			"11001011100",
			"11001001110",
			"11011100100",
			"11001110100",
			"11101101110",
			"11101001100",
			"11100101100",
			"11100100110",
			"11101100100",
			"11100110100",
			"11100110010",
			"11011011000",
			"11011000110",
			"11000110110",
			"10100011000",
			"10001011000",
			"10001000110",
			"10110001000",
			"10001101000",
			"10001100010",
			"11010001000",
			"11000101000",
			"11000100010",
			"10110111000",
			"10110001110",
			"10001101110",
			"10111011000",
			"10111000110",
			"10001110110",
			"11101110110",
			"11010001110",
			"11000101110",
			"11011101000",
			"11011100010",
			"11011101110",
			"11101011000",
			"11101000110",
			"11100010110",
			"11101101000",
			"11101100010",
			"11100011010",
			"11101111010",
			"11001000010",
			"11110001010",
			"10100110000",
			"10100001100",
			"10010110000",
			"10010000110",
			"10000101100",
			"10000100110",
			"10110010000",
			"10110000100",
			"10011010000",
			"10011000010",
			"10000110100",
			"10000110010",
			"11000010010",
			"11001010000",
			"11110111010",
			"11000010100",
			"10001111010",
			"10100111100",
			"10010111100",
			"10010011110",
			"10111100100",
			"10011110100",
			"10011110010",
			"11110100100",
			"11110010100",
			"11110010010",
			"11011011110",
			"11011110110",
			"11110110110",
			"10101111000",
			"10100011110",
			"10001011110",
			"10111101000",
			"10111100010",
			"11110101000",
			"11110100010",
			"10111011110",
			"10111101110",
			"11101011110",
			"11110101110",
			"11010000100",//START A
			"11010010000", //START B
			"11010011100" //START C
		};
		line_max_len = 1024;
	}
public :
    BarcodeCode128(char *str,char *dispStr)
    {
		init();
        line_buf = (char *)malloc(1024);
        disp_str = (char *)malloc(1024);
        char *tem_buf = (char *)malloc(1024);
        memset(line_buf,0,1024);
        memset(disp_str,0,1024);
        str_len = strlen(str);
        need_free  = false;
		
		int i;
        for(i=0;i<str_len;i++)
        {
            if(str[i]>127)goto error;
            if(str[i]<0) goto error;
        }

        memset(tem_buf,0,1024);
        if(str[0]=='{')
        {
            if(((str[1]>=0x31)&&(str[1]<=0x34))||((str[1]>=0x41)&&(str[1]<=0x43))||(str[1]==0x53))
            {
                int tem = 2;
                for(i=2;i<str_len;i++)
                {
                    tem_buf[i-tem]=str[i];
                    if((str[i]=='{')&&(str[i+1]=='{'))
                    {
                        tem++;
                        i++;
                    }
                }
            }
            else goto error;

            memset(str,0,str_len);
            strcpy(str,tem_buf);
            free(tem_buf);
            str_len = strlen(str);
        }


        char dstc[48*2+4];
        int len;
        len=barcode_code128_encode_auto_encode(str, str_len, dstc);
        if (len==0) goto error;
        else if (len > 2*(34-2))  // 384/11=34
            goto error; //数字可能是2个为一个

        barcode_make_CODE128_std(dstc,len,line_buf,line_max_len,disp_str);

        bit_len = strlen(line_buf);
        for(i=0;i<bit_len;i++)
        {
            if(line_buf[i]=='1')line_buf[i/8]|=1<<(i%8);
            else line_buf[i/8]&=~(1<<(i%8));
        }
        for(i=bit_len;i<bit_len+8;i++)
        {
            line_buf[i/8]&=~(1<<(i%8));
        }
        Width = bit_len;
        Height = 1;
        pData = (uint8_t *)line_buf;
        need_free = true;
        if(dispStr)strcpy(dispStr,disp_str);
		if(tem_buf!=NULL)free(tem_buf);
        free(disp_str);
        return ;
    error:
        free(tem_buf);
		pData = NULL;
        need_free = false;
        Width = 1;
        Height = 1;
        free(disp_str);
		free(line_buf);
    }

private :
    int IsCodeB(char ch){return (ch>=32 && ch<=127);}
    int IsNotCodeB(char ch) {return (ch <= 31); }
    int IsCodeA(char ch) {return (ch <= 95);}
    int IsNotCodeA(char ch) {return (ch >= 96 && ch <= 127);}
    int IsNumber(char strNumber){return (strNumber >= '0' && strNumber <= '9');}
    int  get_code128chr_index(int type,unsigned char c)
    {
        if(type==0) //type A
        {
            if(c>=0x20&&c<=0x5f) return (int)c-0x20;
            else if(c<0x20) return (int)c+0x40;
            else if(c==0x80) return 103;
            else if(c==0x81) return  104;
            else if(c==0x82) return  105;
            else if(c==0x83) return  102;
            else if(c==0x84) return  97;
            else if(c==0x85) return  96;
            else if(c==0x86) return  101;
            else if(c==0x88) return  100;
            else if(c==0x89) return  99;
            else if(c==0x90) return  98;
            else return 255;
        }
        else if(type==1) //type B
        {
            if(c>=0x20&&c<=0x7f) return (int)c-0x20;
            else if(c==0x80) return 103;
            else if(c==0x81) return  104;
            else if(c==0x82) return  105;
            else if(c==0x83) return  102;
            else if(c==0x84) return  97;
            else if(c==0x85) return  96;
            else if(c==0x86) return  100;
            else if(c==0x87) return  101;
            else if(c==0x89) return  99;
            else if(c==0x90) return  98;
            else return 255;
        }
        else if(type==2) //type C
        {
            if(c<100) return c;
            else if(c==0x80) return 103;
            else if(c==0x81) return  104;
            else if(c==0x82) return  105;
            else if(c==0x83) return  102;
            else if(c==0x87) return  101;
            else if(c==0x88) return  100;
            else return 255;
        }
        else return 255;
    }

    int barcode_code128_addchar(int type,char *dot_buf,char ch,int max_width)
    {
        int index = get_code128chr_index(type,ch);
        if(index==255) return 0;
        if( (int)(strlen(dot_buf)+strlen(Code128Rule[index]))>max_width) return 0;
        strcat(dot_buf,Code128Rule[index]);
        return 1;
    }
    int barcode_make_CODE128_std(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int i,j,k;
        unsigned char c;
        int type=0;
        char Cc[3];
        int code_type=0;

        for(i=j=0;i<str_len;i++)
        {
            if(j>(line_max_len/8)) break;
            if(str[i]=='{')
            {
                if(str[i+1]=='C')
                {
                    code_type=1;disp_code[j++]=0x00;
                    if (str[i+2]=='{' && str[i+3]=='1'){ i+=2;}
                }
                else if (str[i+1]=='{')
                {	code_type=0;disp_code[j++]='{';	}
                else
                {	code_type=0;disp_code[j++]=0x00;	}
                i++;
            }
            else
            {
                if(code_type==1)
                {	disp_code[j++]=((unsigned char)str[i])*256+(unsigned char)str[i+1];	i++; }
                else
                {	disp_code[j++]=str[i];	}
            }
        }
        if(j>0)	j--;

        for(i=k=0;i<j;i++)
        {
            if(disp_code[i]!=0)
            {
                if(disp_code[i]&0xFF00)
                {	disp_str[k++]=disp_code[i]/256; disp_str[k++]=disp_code[i]%256;	}
                else
                {	disp_str[k++]=(char)disp_code[i];	}
            }
        }
        disp_str[k]=0;

        if(str[0]!='{') goto error;
        switch(str[1])
        {
            case 'A':
                type=0;c=0x80;break;
            case 'B':
                type=1;c=0x81;break;
            case 'C':
                type=2;c=0x82;break;
            default: goto error;
        }

        line_buf[0]=0;
        barcode_code128_addchar(type,line_buf,c,line_max_len);
        for(i=2;i<str_len;i++)
        {
            if(str[i]=='{')
            {
                switch(str[i+1])
                {
                    case 'A':
                        if(type!=0)
                        {
                            c=0x87;
                            if(!barcode_code128_addchar(type,line_buf,c,line_max_len)) break;
                        }
                        type=0;
                        i++;continue;
                    case 'B':
                        if(type!=1)
                        {	c=0x88;if(!barcode_code128_addchar(type,line_buf,c,line_max_len)) break;}
                        type=1;
                        i++;continue;
                    case 'C':
                        if(type!=2)
                        {	c=0x89;	if(!barcode_code128_addchar(type,line_buf,c,line_max_len)) break;}
                        type=2;
                        i++;continue;
                    case '1':
                        c=0x83;i++;break;
                    case '2':
                        c=0x84;i++;break;
                    case '3':
                        c=0x85;i++;break;
                    case '4':
                        c=0x86;i++;break;
                    case 'S':
                        c=0x90;i++;break;
                    case '{':
                        c='{';i++;break;
                    default: goto error;
                }
                if(!barcode_code128_addchar(type,line_buf,c,line_max_len)) break;
                continue;
            }
            else if(type==2)
            {
                Cc[0]=str[i];
                Cc[1]=str[i+1];
                Cc[2]=0;
                if(Cc[0]<'0'||Cc[0]>'9'||Cc[1]<'0'||Cc[1]>'9') goto error;
                c = (Cc[0]-'0')*10+(Cc[1]-'0');
                if(!barcode_code128_addchar(type,line_buf,c,line_max_len)) break;
                i++;continue;
            }
            else
            {
                c=str[i];
                if(!barcode_code128_addchar(type,line_buf,c,line_max_len)) break;
            }
        }
        if((int)(strlen(line_buf)+13)<line_max_len) strcat(line_buf,"1100011101011");
        return 1;
    error:
        disp_str[0]=0;
        return 0;
    }

    int IsContinueNum(char* buf, int index, int max)
    {
        if (index + 3 > max) return 0; //防止数据访问越界
        return (IsNumber(buf[index+0]) && IsNumber(buf[index + 1]) && IsNumber(buf[index + 2]) &&
            IsNumber(buf[index + 3]));
    }

    int barcode_code128_encode_auto_encode(char* indata, int inlen, char* dstc)
    {
        int last_code_type = 0;
        int i = 0;
        int len = 0;
        int checkcodeID = 0;
        int k;

        int CodeType = 0;//CODE128_FORMAT_A

        if (inlen>80) return 0;//最多80个字符。
        //sys_watchdog_clear();
        if (IsContinueNum(indata, i, inlen))
        {CodeType = CODE128_FORMAT_C; dstc[len++] = '{'; dstc[len++] = 'C'; checkcodeID = V_START_C;}
        else if (IsCodeB(indata[i]))
        {CodeType = CODE128_FORMAT_B; dstc[len++] = '{'; dstc[len++] = 'B'; checkcodeID = V_START_B;}
        else if (IsCodeA(indata[i]))
        {CodeType = CODE128_FORMAT_A; dstc[len++] = '{'; dstc[len++] = 'A'; checkcodeID = V_START_A;}
        else return 0;

        for (i = 0,k=1; i < inlen; )
        {
            if (indata[i] > 127 ) return 0;//CODE128  范围0~127
            switch (CodeType)
            {
                case 3://CODE128_FORMAT_C:
                    last_code_type = CODE128_FORMAT_C;
                    for (; i < inlen; i+=2)
                    {
                        if (IsNumber(indata[i]) && IsNumber(indata[i + 1]))
                        {
                            dstc[len++] = indata[i]; dstc[len++] = indata[i + 1];
                            checkcodeID += (k++) * ((indata[i] - 0x30) * 10 + (indata[i+1] - 0x30));
                        }
                        else
                        {
                            if ((IsNumber(indata[i]) && IsCodeB(indata[i + 1])) || IsCodeB(indata[i]))
                            {
                                CodeType = CODE128_FORMAT_B;
                                dstc[len++] = '{'; dstc[len++] = 'B';
                                checkcodeID += V_CODE_B * (k++);
                                break;
                            }
                            else if ((IsNumber(indata[i]) && IsCodeA(indata[i + 1])) || IsCodeA(indata[i]))
                            {
                                CodeType = CODE128_FORMAT_A;
                                dstc[len++] = '{'; dstc[len++] = 'A';
                                checkcodeID += V_CODE_A * (k++);
                                break;
                            }
                        }
                    }
                    break;
                case 2://CODE128_FORMAT_B
                    last_code_type = CODE128_FORMAT_B;
                    for (; i < inlen; i++)
                    {
                        if (IsContinueNum(indata, i, inlen))
                        {
                            CodeType = CODE128_FORMAT_C;
                            dstc[len++] = '{'; dstc[len++] = 'C';
                            checkcodeID += V_CODE_C * (k++);
                            break;
                        }
                        else if (IsNotCodeB(indata[i]))
                        {
                            CodeType = CODE128_FORMAT_A;
                            dstc[len++] = '{'; dstc[len++] = 'A';
                            checkcodeID += V_CODE_A * (k++);
                            break;
                        }
                        else
                        {
                            if (indata[i] == '{') { dstc[len++] = '{'; dstc[len++] = '{'; }
                            else  { dstc[len++] = indata[i]; }
                            checkcodeID += (k++) * (indata[i] - 32);
                        }
                    }
                    break;
                case 1://CODE128_FORMAT_A:
                    last_code_type = CODE128_FORMAT_A;
                    for (; i < inlen; i++)
                    {
                        if (IsContinueNum(indata, i, inlen))
                        {
                            CodeType = CODE128_FORMAT_C;
                            dstc[len++] = '{'; dstc[len++] = 'C';
                            checkcodeID += V_CODE_C * (k++);
                            break;
                        }
                        else if (IsNotCodeA(indata[i]))
                        {
                            CodeType = CODE128_FORMAT_B;
                            dstc[len++] = '{'; dstc[len++] = 'B';
                            checkcodeID += V_CODE_B * (k++);
                            break;
                        }
                        else
                        {
                            dstc[len++] = indata[i];
                            int ch = indata[i];
                            if (ch >= 32 && ch <= 95)   checkcodeID += (k++) * (ch- 32);
                            else if (ch <= 31 && ch >= 0)   checkcodeID += (k++) * (ch + 64);
                        }
                    }
                    break;
                default: return 0;
                }
            }
            checkcodeID = checkcodeID % 103;
            switch (last_code_type)// 处理校验和
            {
                case 1://CODE128_FORMAT_A:
                    if (checkcodeID >= 0 && checkcodeID <= 63) //0~63
                        dstc[len++] = (checkcodeID + 32);
                    else if (checkcodeID >= 64 && checkcodeID <= 95) //64~95
                        dstc[len++] = checkcodeID - 64;
                    else if (checkcodeID == 96) { dstc[len++] = '{'; dstc[len++] = '3'; }
                    else if (checkcodeID == 97) { dstc[len++] = '{'; dstc[len++] = '2'; }
                    else if (checkcodeID == 98) { dstc[len++] = '{'; dstc[len++] = 'S'; }
                    else if (checkcodeID == 99) { dstc[len++] = '{'; dstc[len++] = 'C'; }
                    else if (checkcodeID == 100){ dstc[len++] = '{'; dstc[len++] = 'B'; }
                    else if (checkcodeID == 101){ dstc[len++] = '{'; dstc[len++] = '4'; }
                    else if (checkcodeID == 102){ dstc[len++] = '{'; dstc[len++] = '1'; }
                    break;
                case 2://CODE128_FORMAT_B:
                    if (checkcodeID >= 0 && checkcodeID <= 95)
                    {
                        if (checkcodeID==0x5B) //转义'{'
                            { dstc[len++] = '{'; dstc[len++] = '{'; }
                        else   dstc[len++] = (checkcodeID + 32);
                    }
                    else if (checkcodeID == 96) { dstc[len++] = '{'; dstc[len++] = '3'; }
                    else if (checkcodeID == 97) { dstc[len++] = '{'; dstc[len++] = '2'; }
                    else if (checkcodeID == 98) { dstc[len++] = '{'; dstc[len++] = 'S'; }
                    else if (checkcodeID == 99) { dstc[len++] = '{'; dstc[len++] = 'C'; }
                    else if (checkcodeID == 100){ dstc[len++] = '{'; dstc[len++] = '4'; }
                    else if (checkcodeID == 101){ dstc[len++] = '{'; dstc[len++] = 'A'; }
                    else if (checkcodeID == 102){ dstc[len++] = '{'; dstc[len++] = '1'; }
                    break;
                case 3://CODE128_FORMAT_C:
                    if (checkcodeID >= 0 && checkcodeID <= 99)
                    { dstc[len++] =(checkcodeID / 10 + 48);dstc[len++] = (checkcodeID % 10 + 48); }
                    else if (checkcodeID == 100) { dstc[len++] = '{'; dstc[len++] = 'B'; }
                    else if (checkcodeID == 101) { dstc[len++] = '{'; dstc[len++] = 'A'; }
                    else if (checkcodeID == 102) { dstc[len++] = '{'; dstc[len++] = '1'; }
                    break;
                default:    break;
            }
        return len;
    }

};





#endif // BARCODE_CODE128_H














