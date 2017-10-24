#include <mbed.h>
#include "CmdBase.h"
#include "Textout.h"
#include "PrnCmd.h"

VAR_REG_U32(GOTO_MARK_LEN,GOTO_MARK_LEN,300*8,"GOTO_MARK_LEN");

CMD_0x1B(0x4A)	//6. ESC 'J'		��ӡ����ֽn����λ����
{
	if(CmdLen<3)return CMD_CONTINUE;
	int line_height = Cmd[2];
	if(line_height>1016*8) line_height = 1016*8;   //���ֽ1016mm
	textout->print_line();
	prnCmd->feed(line_height);
	return CMD_FINISH;
}

CMD_0x1B(0x64)	//10. ESC 'd'		��ӡ����ֽn�ַ���
{
	if(CmdLen<3)return CMD_CONTINUE;
	int n=Cmd[2];
	textout->print_line();
	for(int i=0;i<n;i++)textout->print_line(1);
	return CMD_FINISH;
}

CMD_0x1D(0x69)		//				��ӡ����ֽ���Һڱ괦
{
	textout->print_line();
	prnCmd->goto_mark_right(GOTO_MARK_LEN);
	return CMD_FINISH;
}

CMD_0x1D(0x0C)	//x. GS 0x0C		��ֽ��LABEL��
{
	textout->print_line();
	prnCmd->goto_mark_label(GOTO_MARK_LEN);
	return CMD_FINISH;
}

CMD(0x0E)		//x. SO				��ӡ����ֽ���Һڱ괦
{
	textout->print_line();
	prnCmd->goto_mark_right(GOTO_MARK_LEN);
	return CMD_FINISH;
}

CMD(0x0C)		//6. FF				��ӡ����ֽ����ڱ�
{
	textout->print_line();
	prnCmd->goto_mark_left(GOTO_MARK_LEN);
	return CMD_FINISH;
}
