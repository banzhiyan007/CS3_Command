#ifndef TEXTOUT_H
#define TEXTOUT_H

#include <Block.hpp>
#include <Font.hpp>
#include "Canvas.h"

class Textout
{
public:
	int cur_x;
	int cur_y;
	uint16_t two_byte_char;
	TextStyle *textStyle;
	int margin_left;
	int margin_right;
	int align;
	int charspace_asc;
	int charspace_multi_left;
	int charspace_multi_right;
	int blank_line_height;
	int line_height;
	int line_space;
	int default_char_width;
	uint8_t *tmp;
public:
	Textout();
	void reset();
	void reset_codepage(void);
	void out(uint8_t ch);
	void print_line(bool print_blank_line=false);
};
extern Textout *textout;

struct TEXTOUT_FUNC_LIST_NODE
{
	void (*func)(void);
	TEXTOUT_FUNC_LIST_NODE *pNext;
};
extern TEXTOUT_FUNC_LIST_NODE *textout_reset_func;
#define TEXTOUT_RESET_FUNC_ADD(f)	{static TEXTOUT_FUNC_LIST_NODE node={f,NULL};node.pNext=textout_reset_func;textout_reset_func=&node;}

extern uint32_t EPSON_MODE;

#endif

