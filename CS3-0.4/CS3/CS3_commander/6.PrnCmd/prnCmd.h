#ifndef PRN_CMD_H
#define PRN_CMD_H

class PrnCmd
{
public:
	PrnCmd();
	void feed(uint16_t linecount);
	void feed_backward(uint16_t linecount);
	void goto_mark_left(uint16_t linecount);
	void goto_mark_right(uint16_t linecount);
	void goto_mark_label(uint16_t linecount);
	void goto_mark_all(uint16_t linecount);
	void delay(uint16_t ms);
	void print_line(void *pLineData,int Len);
	void set_darkness(int darkness);
	void set_speed(int speed);
	void get_mark_his(int his_len);
};
extern PrnCmd *prnCmd;

#endif

