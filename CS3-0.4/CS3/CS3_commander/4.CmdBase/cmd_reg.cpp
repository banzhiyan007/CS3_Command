#include "CmdBase.h"
#include "LinkReg.h"

CMD_0x1F(0x52)	//REG READ WRITE COMMAND
{
	if(CmdLen < 4)return CMD_CONTINUE;
	static uint16_t len;
	if(CmdLen ==4){len = Cmd[2]+Cmd[3]*256;}
	if(CmdLen < 4+len)return CMD_CONTINUE;

	switch(Cmd[4])
	{
        case 0x80:		// get reg count
            {
                uint32_t RegCount = link_reg_count();
                char buf[5];
                buf[0]=0x80;
                buf[1]=0x02;
                buf[2]=0x00;
                memcpy(&buf[3],&RegCount,2);
                tx(buf,3+2);
                break;
            }
        case 0x81:		// get reg info
            {
                uint32_t index = Cmd[5]+Cmd[6]*256;
				char pName[64];
				char pInfo[64];
				strncpy(pName,link_reg_name(index),sizeof(pName));
				strncpy(pInfo,link_reg_info(index),sizeof(pInfo));
                char buf[6];
                buf[0]=0x81;
                buf[1]=0x03+strlen(pName)+1+strlen(pInfo)+1;
                buf[2]=0x00;
                buf[3]=link_reg_type(index);
                buf[4]=link_reg_size(index)%256;
                buf[5]=link_reg_size(index)/256;
                tx(buf,6);
                tx((char*)pName,strlen(pName)+1);
                tx((char*)pInfo,strlen(pInfo)+1);
                break;
            }
        case 0x20:		// read reg value
            {
                char *pName = (char *)&Cmd[6];
                switch(Cmd[5])
                {
                    case 0x00:
                        {
                            int index=link_reg_index(pName);
                            char buf[7];
                            uint32_t value;
                            if(index>=0)
                            {
                                value = link_reg_get_u32_by_index(index);
                                buf[0]=0x20;
                                buf[1]=0x04;
                                buf[2]=0x00;
                                memcpy(&buf[3],&value,4);
                                tx(buf,7);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                    case 0x01:
                        {
                            int index=link_reg_index(pName);
                            char buf[3];
                            char *pValue;
                            if(index>=0)
                            {
                                pValue = link_reg_get_string_by_index(index);
                                buf[0]=0x20;
                                buf[1]=strlen(pValue)+1;
                                buf[2]=0x00;
                                tx(buf,3);
                                tx(pValue,strlen(pValue)+1);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                    case 0x03:
                        {
                            char buf[3];
                            int index=link_reg_index(pName);
                            unsigned char *pValue;
                            if(index>=0)
                            {
								uint8_t pValue[64];
								int size=link_reg_size(index);
								memcpy(pValue,link_reg_get_bytes_by_index(index),size);
                                buf[0]=0x20;
                                buf[1]=size;
                                buf[2]=0x00;
                                tx(buf,3);
                                tx((char*)pValue,size);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                    case 0x04:  //get buffer
                        {
                            uint16_t  data_pos = (Cmd[6]+Cmd[7]*256);
                            uint16_t  data_len = (Cmd[8]+Cmd[9]*256);
                            char *pName = (char *)&Cmd[10];
                            char buf[3];
                            int index=link_reg_index(pName);
                            unsigned char *pValue;
                            if(index>=0)
                            {
                                pValue = link_reg_get_buffer_by_index(index);
                                buf[0]=0x20;
                                buf[1]=data_len%256;
                                buf[2]=data_len/256;
                                tx(buf,3);
                                tx((char*)pValue+data_pos,data_len);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                }
                break;
            }
        case 0x30:		// set reg value
            {
				char pName[32];
                char *pValue = (char *)&Cmd[10]+(Cmd[6]+Cmd[7]*256);
                uint16_t  datalen = (Cmd[8]+Cmd[9]*256);
				memcpy(pName,(char *)&Cmd[10],(Cmd[6]+Cmd[7]*256));
				pName[(Cmd[6]+Cmd[7]*256)]=0;
				//printf("\n\r pName is %s", pName);
                switch(Cmd[5])
                {
                    case 0x00:
                        {
                            int index=link_reg_index(pName);
                            uint32_t value;
							//printf("Test Index is %d, %s\n\r", index, pName);
                            if(datalen!=4)break;
                            memcpy(&value,pValue,4);
                            char buf[3];
                            if(index>=0)
                            {
                                link_reg_set_u32_by_index(index,value);
                                buf[0]=0x30;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                    case 0x01:
                        {
                            int index=link_reg_index(pName);
                            char buf[3];
                            if(index>=0)
                            {
                                link_reg_set_string_by_index(index,pValue);
                                buf[0]=0x30;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                    case 0x02:
                        {
							if(strcmp(pName,"power_reset")==0)
							{
								char buf[3];
								buf[0]=0x30;
								buf[1]=0x00;
								buf[2]=0x00;
								tx(buf,3);
								wait_ms(200);
								link_reg_exec_function("power_reset");
							}
							if(strcmp(pName,"power_off")==0)
							{
								char buf[3];
								buf[0]=0x30;
								buf[1]=0x00;
								buf[2]=0x00;
								tx(buf,3);
								wait_ms(200);
								link_reg_exec_function("power_off");
							}
                            int index=link_reg_index(pName);							
                            char buf[3];
                            if(index>=0)
                            {								
                                buf[0]=0x30;
                                buf[1]=0x00;
                                buf[2]=0x00;
								if(strcmp(pName, "bluetooth_clear_bind") != 0)
									tx(buf,3);
								link_reg_exec_function_by_index(index);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                    case 0x03:
                        {
                            int index=link_reg_index(pName);							
                            char buf[3];
                            if(index>=0)
                            {
                                link_reg_set_bytes_by_index(index,(uint8_t*)pValue,datalen);
                                buf[0]=0x30;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                    case 0x12:
                        {
                            int index=link_reg_index(pName);
                            char buf[3];
                            if(index>=0)
                            {
                                link_reg_exec_function_by_index(index);
                                buf[0]=0x30;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                            else
                            {
                                buf[0]=0x00;
                                buf[1]=0x00;
                                buf[2]=0x00;
                                tx(buf,3);
                            }
                        }
                        break;
                }
                break;
            }
        default:break;
    }
	return CMD_FINISH;
}

