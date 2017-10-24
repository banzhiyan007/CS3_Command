#ifndef PDF417_H
#define PDF417_H

#include <Block.hpp>

class PDF417 : public  Block
{
private :
	int SIZE;
	int PDF417_BUF_SIZE;
	char *p_strdatatext;
	int m_rownumber;
	int m_colnumber;
	short* m_errorarray;
	int* codearray;
	int *Lt;
	int *Rt;
	short* xi;
	unsigned int m_codenumber;
	int m_errornumber;
	int m_errorcodenumber;
	unsigned short m_textlen;
	int m_begin;
	int m_end;
	int m_hwratio;
	int *tempr;
	int *tempq;
	int *temp;
	int *inttemp;
private :
	int CEIL(int a, int b);
	char ReverseCharBits(char Num);
	void Init(int colnumber,int error,int hwratio,char *buf,int buf_len);
	void AddTailNum();
	void GetLAndR(int colnumber,int rownumber,int errornumber);
	void GetError();
	int CalErrorCode();
	void EncodeBar();
	void EncodeTC(int begin, int length, int precodenumber);
	void byteCompaction6(int begin,int precodenumber);
	void byteCompaction(int begin, int length,int precodenumber);
	void basicNumberCompaction(int begin, int length,int percodenumber);
	void numberCompaction(int begin, int length,int percodenumber);
	void BitConvert();
	void TotalModel();
	int OnDraw();
private:
	char *pOutBuf;
	unsigned char *pdf417buf;
	int _colnumber;
	int _error;
	int _hwratio;
	int code_len;
public:
	PDF417(int colnumber,int error,int hwratio,char *code);
};


#endif

