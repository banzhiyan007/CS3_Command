#ifndef BARCODE_EAN_H
#define BARCODE_EAN_H
#include "mbed.h"

class BarcodeEAN: public Block
{
private :
    const char *UPCRule[10][3];
    const char *UPC_ENA13_ArrayRule[10];
    const char *UPC_E_ArrayRule[10][2];
	int line_max_len;

    int str_len;
    char *line_buf;
    char *disp_str;
    int bit_len;
	void init(void)
	{
		UPCRule=
		{
			//   Left 1     left 2     right
			{"0001101","0100111","1110010"},//0
			{"0011001","0110011","1100110"},//1
			{"0010011","0011011","1101100"},//2
			{"0111101","0100001","1000010"},//3
			{"0100011","0011101","1011100"},//4
			{"0110001","0111001","1001110"},//5
			{"0101111","0000101","1010000"},//6
			{"0111011","0010001","1000100"},//7
			{"0110111","0001001","1001000"},//8
			{"0001011","0010111","1110100"} //9
		};

		UPC_ENA13_ArrayRule=
		{
			"000000",//0
			"001011",//1
			"001101",//2
			"001110",//3
			"010011",//4
			"011001",//5
			"011100",//6
			"010101",//7
			"010110",//8
			"011010" //9
		};

		UPC_E_ArrayRule=
		{
			{"111000","000111"},
			{"110100","001011"},
			{"110010","001101"},
			{"110001","001110"},
			{"101100","010011"},
			{"100110","011001"},
			{"100011","011100"},
			{"101010","010101"},
			{"101001","010110"},
			{"100101","011010"}
		};
		line_max_len = 1024;
	}

public :
    BarcodeEAN(char *type,char *str,char *dispStr)
    {
		init();
        line_buf = (char *)malloc(1024);
        disp_str = (char *)malloc(1024);
        memset(line_buf,0,1024);
        memset(disp_str,0,1024);
        need_free = false;
        str_len = strlen(str);

        int i;
        if(strcmp("EAN8",type)==0)
        {
            char str1[9];
            memset(str1,0,9);
            if((str_len<7)||(str_len>8)) goto err;
            for(i=0;i<str_len;i++)if(str[i]<'0'||str[i]>'9') goto err;
            memcpy(str1,str,7);
            str1[7]= EAN_checksum(str,7);
            barcode_make_EAN8_std(str1,8,line_buf,line_max_len,disp_str);
        }
        else if(strcmp("EAN13",type)==0)
        {
            char str2[16];
            memset(str2,0,16);
            if((str_len<12)||(str_len>13)) goto err;
            for(i=0;i<str_len;i++)if(str[i]<'0'||str[i]>'9') goto err;
            memcpy(str2,str,12);
            str2[12] = EAN_checksum(str,12);
            barcode_make_EAN13_std(str2,13,line_buf,line_max_len,disp_str);
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
        if(dispStr)strcpy(dispStr,disp_str);
        free(disp_str);
        return ;
    err:
        need_free = true;
        memset(line_buf,0,1024);
        pData = (uint8_t *)line_buf;
        Width = 1;
        Height = 1;
        free(disp_str);
    }
private :
    int barcode_make_EAN8_std(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int i;
        line_buf[0]=0;
        if(str_len!=8) return 0;
        for(i=0;i<8;i++) if(str[i]<'0'||str[i]>'9') return 0;
        memcpy(disp_str,str,str_len);disp_str[str_len]=0;

        strcat(line_buf,"101");
        for(i=0;i<4;i++)
        {
            if(strlen(line_buf)+strlen(UPCRule[str[i]-'0'][0])>((unsigned int)line_max_len)) return 1;
            strcat(line_buf,UPCRule[str[i]-'0'][0]);
        }
        if(strlen(line_buf)+5>(unsigned int)line_max_len) return 1;
        strcat(line_buf,"01010");
        for(i=4;i<8;i++)
        {
            if(strlen(line_buf)+strlen(UPCRule[str[i]-'0'][2])>((unsigned int)line_max_len)) return 1;
            strcat(line_buf,UPCRule[str[i]-'0'][2]);
        }
        if(strlen(line_buf)+10>((unsigned int)line_max_len)) return 1;
        strcat(line_buf,"101");
        return 1;
    }
    char EAN_checksum(char *str,int str_len)
    {
        int i;
        int check_sum = 0;

        if (str_len%2==1) //奇数个
        {
            for(i=0;i<str_len;i++)
            {
                if(i%2==0)	check_sum += (str[i]-'0')*3;
                else	check_sum += (str[i]-'0');
            }
        }
        else //偶数个
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

    int barcode_make_EAN13_std(char *str,int str_len,char *line_buf,int line_max_len,char *disp_str)
    {
        int i;
        int ArrayR[6];
        if(str_len!=13) return 0;
        for(i=0;i<13;i++)	if(str[i]<'0'||str[i]>'9') return 0;

        memcpy(disp_str,str,str_len);disp_str[str_len]=0;
        line_buf[0]=0;

        strcat(line_buf,"101");
        for(i=0;i<6;i++)	ArrayR[i]=UPC_ENA13_ArrayRule[str[0]-'0'][i]-'0';

        for(i=1;i<7;i++)
        {
            if(strlen(line_buf)+strlen(UPCRule[str[i]-'0'][ArrayR[i-1]])>((unsigned int)line_max_len)) return 1;
            strcat(line_buf,UPCRule[str[i]-'0'][ArrayR[i-1]]);
        }
        if(strlen(line_buf)+5>((unsigned int)line_max_len)) return 1;
        strcat(line_buf,"01010");
        for(i=7;i<13;i++)
        {
            if(strlen(line_buf)+strlen(UPCRule[str[i]-'0'][2])>((unsigned int)line_max_len)) return 1;
            strcat(line_buf,UPCRule[str[i]-'0'][2]);
        }
        if(strlen(line_buf)+12>((unsigned int)line_max_len)) return 1;
        strcat(line_buf,"101");
        return 1;
    }




};

#endif
















