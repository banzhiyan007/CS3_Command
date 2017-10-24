#ifndef BARCODE_CODEBAR_H
#define BARCODE_CODEBAR_H
#include "mbed.h"

class BarcodeCodabar : public Block
{
private :
    const char *CodabarRule[21];
	int line_max_len;
    int str_len;
    char *line_buf;
    char *disp_str;
    int bit_len;
	void init(void)
	{
		CodabarRule=
		{
			"1010100110", //'0'
			"1010110010", //'1'
			"1010010110", //'2'
			"1100101010", //'3'
			"1011010010", //'4'
			"1101010010", //'5'
			"1001010110", //'6'
			"1001011010", //'7'
			"1001101010", //'8'
			"1101001010", //'9'
			"1010011010", //'-'
			"1011001010", //'$'
			"11010110110", //':'
			"11011010110", //'/'
			"11011011010", //'.'
			"10110110110", //'+'
			"10110010010", //'A' stop/start
			"10010010110",  //'B' stop/start
			"10100100110",  //'C' stop/start
			"10100110010",   //'D' stop/start
			"0000000000" //NULL0
		};
		line_max_len = 1024;
	}
public :
    BarcodeCodabar(char *str,char *dispStr)
    {
		init();
        line_buf = (char *)malloc(1024);
        disp_str = (char *)malloc(1024);
        memset(line_buf,0,1024);
        memset(disp_str,0,1024);
        str_len = strlen(str);
        need_free = false;

        int i=0;
        int add_flag = 1;
        char ch;
        int checksum;
        if (str_len>48) goto err;
        for (i=0; i <str_len; i++)
        { if (barcode_codabar_char_index(str[i])==20) goto err;}
        for (i = 1; i < str_len-1; i++)
        { if (barcode_codabar_char_index(str[i])>15) goto err;}

        ch=str[0];
        if (ch=='A' ||ch=='B' || ch=='C' || ch=='D')
        {
            if (str_len<3) goto err;
            ch=str[str_len-1];
            if (ch=='A' || ch=='B' || ch=='C' || ch=='D'){add_flag=0;}
            else goto err;
        }
        else
        {
            ch=str[str_len-1];
            if (ch=='A' || ch=='B' || ch=='C' || ch=='D') goto err;
        }
        if (add_flag) {memcpy(disp_str,str,str_len);disp_str[str_len]=0;}
        else {memcpy(disp_str,str+1,str_len-2);disp_str[str_len-2]=0;}

        line_buf[0]=0;
        checksum=0;
        if (add_flag)
        {
            barcode_codabar_addchar(line_buf,'A',line_max_len);
            checksum+=barcode_codabar_char_index('A');

            for(i=0;i<str_len;i++)
            {
                if (!barcode_codabar_addchar(line_buf,str[i],line_max_len)) goto err;
                checksum+=barcode_codabar_char_index(str[i]);
            }
            checksum+=barcode_codabar_char_index('B');
            checksum=checksum%16;
            if (checksum) checksum=16-checksum;

            //if((int)(strlen(line_buf)+strlen(CodabarRule[checksum]))>=line_max_len) goto err; //Add checksum
            //else strcat(line_buf,CodabarRule[checksum]);

            barcode_codabar_addchar(line_buf,'B',line_max_len);
        }
        else
        {
            for(i=0;i<str_len-1;i++)
            {
                if (!barcode_codabar_addchar(line_buf,str[i],line_max_len)) goto err;
                checksum+=barcode_codabar_char_index(str[i]);
            }
            ch=str[str_len-1];
            checksum+=barcode_codabar_char_index(ch);

            checksum=checksum%16;
            if (checksum) checksum=16-checksum;
            //if((int)(strlen(line_buf)+strlen(CodabarRule[checksum]))>=line_max_len) goto err; //Add checksum
            //else strcat(line_buf,CodabarRule[checksum]);

            barcode_codabar_addchar(line_buf,ch,line_max_len);
        }

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
        if(dispStr)
		{
			dispStr[0]='A';
			strcpy(dispStr+1,disp_str);
			strcat(dispStr,"B");
		}
        free(disp_str);
        return ;
    err:
        need_free = true;
        pData = (uint8_t *)line_buf;
        memset(line_buf,0,1024);
        Width = 1;
        Height = 1;
        free(disp_str);
    }

private :
    int barcode_codabar_char_index(char ch)
    {
        int index;
        if    (ch>='0'&&ch<='9')index=ch-48;
        else if(ch=='-')index=10;
        else if(ch=='$')index=11;
        else if(ch==':')index=12;
        else if(ch=='/')index=13;
        else if(ch=='.')index=14;
        else if(ch=='+')index=15;
        else if(ch=='A')index=16;
        else if(ch=='B')index=17;
        else if(ch=='C')index=18;
        else if(ch=='D')index=19;
        else index=20;
        return index;
    }

    int barcode_codabar_addchar(char *dot_buf,char ch,int max_width)
    {
        int index = barcode_codabar_char_index(ch);
        if((int)(strlen(dot_buf)+strlen(CodabarRule[index]))>=max_width) return 0;
        strcat(dot_buf,CodabarRule[index]);
        return 1;
    }

    int barcode_make_CODABAR_std_auto(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int i=0;
        int add_flag = 1;
        disp_str[0]=0;
        char ch;
        if (str_len>48) return 0;

        for (i=0; i <str_len; i++)
        { if (barcode_codabar_char_index(str[i])==20) return 0;}
        for (i = 1; i < str_len-1; i++)
        { if (barcode_codabar_char_index(str[i])>15) return 0;}

        ch=str[0];
        if (ch=='A' ||ch=='B' || ch=='C' || ch=='D')
        {
            if (str_len<3) return 0;
            ch=str[str_len-1];
            if (ch=='A' || ch=='B' || ch=='C' || ch=='D'){add_flag=0;}
            else return 0;
        }
        else
        {
            ch=str[str_len-1];
            if (ch=='A' || ch=='B' || ch=='C' || ch=='D') return 0;
        }
        if (add_flag) {memcpy(disp_str,str,str_len);disp_str[str_len]=0;}
        else {memcpy(disp_str,str+1,str_len-1);disp_str[str_len-1]=0;}

        line_buf[0]=0;
        if (add_flag)barcode_codabar_addchar(line_buf,'A',line_max_len);
        for(i=0;i<str_len;i++)
        {
            if (!barcode_codabar_addchar(line_buf,str[i],line_max_len)) break;
        }
        if (add_flag)barcode_codabar_addchar(line_buf,'B',line_max_len);
        return 1;
    }

    int barcode_make_CODABAR_std(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int j=0;
        disp_str[0]=0;
        if (str_len>48) return 0;
        if ( !(str[0]=='A' || str[0]=='B' || str[0]=='C' || str[0]=='D') )return 0;
        if ( !(str[str_len-1]=='A' || str[str_len-1]=='B' || str[str_len-1]=='C' || str[str_len-1]=='D')) return 0;

        memcpy(disp_str,str,str_len);disp_str[str_len]=0;
        line_buf[0]=0;
        for(j=0;j<str_len;j++)
        {
            if (!barcode_codabar_addchar(line_buf,str[j],line_max_len)) break;
        }
        return 1;
    }

};



#endif






