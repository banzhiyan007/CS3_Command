#ifndef BARCODE_ITF_H
#define BARCODE_ITF_H
#include "mbed.h"

class BarcodeITF : public Block
{
private :
    const char *ITFRule[10];
	int line_max_len;
    int str_len;
    char *line_buf;
    char *disp_str;
    int bit_len;
	void init(void)
	{
		ITFRule=
		{
			"NNWWN", //'0'
			"WNNNW", //'1'
			"NWNNW", //'2'
			"WWNNN", //'3'
			"NNWNW", //'4'
			"WNWNN", //'5'
			"NWWNN", //'6'
			"NNNWW", //'7'
			"WNNWN", //'8'
			"NWNWN"  //'9'
		};
		line_max_len = 1024;
	}
public :
    BarcodeITF(char *str,char *dispStr)
    {
		init();
        line_buf = (char *)malloc(1024);
        disp_str = (char *)malloc(1024);
        memset(line_buf,0,1024);
        memset(disp_str,0,1024);
        str_len = strlen(str);
        need_free = false;

        int i;
        char str2[48];
        if (str_len>47) goto err;
        for(i=0;i<str_len;i++)
        {
            if((str[i]<48)||(str[i]>57)) goto err;
        }
        str_len = (str_len/2)*2;// 取最大偶数

        memcpy(str2,str,str_len);
        //if(str_len%2==0)if(str_len>1){str2[str_len-1]=0;str_len--;}
        //str2[str_len] = EAN_checksum(str2,str_len);
        //str2[str_len+1]=0;
        str2[str_len]=0;
        //barcode_make_ITF_std(str2,str_len+1,line_buf,line_max_len,disp_str);
        barcode_make_ITF_std(str2,str_len,line_buf,line_max_len,disp_str);

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
        free(disp_str);
        return ;
    err:
        pData = (uint8_t *)line_buf;
        need_free = true;
        memset(line_buf,0,1024);
        Width = 1;
        Height = 1;
        free(disp_str);
    }


private :
    int barcode_make_ITF_std(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        char IUnit_Buf[48];
        char buf[48];
        int i,j;
        if(str_len%2==1) return 0;

        memcpy(disp_str,str,str_len);disp_str[str_len]=0;
        line_buf[0]=0;

        strcat(line_buf,"1010");

        for(i=0;i<str_len/2;i++)
        {
            buf[0]=0;
            if(str[2*i]<'0'||str[2*i]>'9'||str[2*i+1]<'0'||str[2*i+1]>'9')
            {
                strcat(buf,"00000000000000");
                continue;
            }

            for(j=0;j<5;j++)
            {
                IUnit_Buf[2*j]=ITFRule[str[2*i]-'0'][j];
                IUnit_Buf[2*j+1]=ITFRule[str[2*i+1]-'0'][j];
            }
            for(j=0;j<10;j++)
            {
                if(IUnit_Buf[j]=='W'&&j%2==0)
                    strcat(buf,"11");
                else if(IUnit_Buf[j]=='W'&&j%2==1)
                    strcat(buf,"00");
                else if(IUnit_Buf[j]=='N'&&j%2==0)
                    strcat(buf,"1");
                else if(IUnit_Buf[j]=='N'&&j%2==1)
                    strcat(buf,"0");
            }
            if(strlen(line_buf)+strlen(buf)>((uint32_t)line_max_len)) return 1;
            strcat(line_buf,buf);
        }
        if(strlen(line_buf)+4>((uint32_t)line_max_len)) return 1;
        strcat(line_buf,"1101");
        return 1;
    }

    char EAN_checksum(char *str,int str_len)
    {
        int i;
        int check_sum = 0;

        if (str_len%2==1) //ÆæÊý¸ö
        {
            for(i=0;i<str_len;i++)
            {
                if(i%2==0)	check_sum += (str[i]-'0')*3;
                else	check_sum += (str[i]-'0');
            }
        }
        else //Å¼Êý¸ö
        {
            for(i=0;i<str_len;i++)
            {
                if(i%2==0)	check_sum += (str[i]-'0');
                else	check_sum += (str[i]-'0')*3;
            }
        }
        check_sum = check_sum%10;
        if(check_sum)check_sum = 10 - check_sum;
        check_sum = '0'+check_sum;
        return check_sum;
    }



};






#endif





