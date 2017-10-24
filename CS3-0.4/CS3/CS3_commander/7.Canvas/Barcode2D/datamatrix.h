#ifndef DATAMATRIX_H
#define DATAMATRIX_H

#include <Block.hpp>

class Datamatrix : public Block
{
private:
	int PLOT_BUF_SIZE;
	int BARCODE_DATA_MAX;
	int BARCODE_BUF_SIZE;
	int nrow,ncol;
	int gIndex;
	int m_codenumber;
	int m_padsize;
	int m_textlen;
	unsigned char*printarray;
	unsigned char *temparray; //size=26*26*6(4056)
	//最初接受数据文本信息，将文本流编码成码字流后，temparray无用可覆盖；
	//最后转化为数据点阵。
	//unsigned char *printchar; //size=18*144(2596)
	unsigned char *p_strdatatext;//size=18*144(2596)
	unsigned char *pDataBuf ;
	int code_len;
public :
	Datamatrix(char *code);
private:
	//编码形成码字流，然后转化为点阵，p_strdatatext无用；
	//将点阵转化成放置后的码字流，temparray无用可覆盖；
	//通过ReverseCharBits转化为打印机可以打印的码字流。
	char ReverseCharBits(char Num);
	////////////////////////初始化
	void Init(char* buf,int len);
	///////////////////编码部分
	unsigned char Randomize255State(unsigned char codewordValue, int codewordPosition);
	bool EncodeBase256(int begin,int textlen,int precodenumber);
	bool Encode();
	/////////////////////编码部分
	///////////////////////////////////////////////////////////////////////////////////////
	//////////////////////选择放置矩阵
	bool ChooseIndex(int codenumber);
	//////////////////////选择放置矩阵
	///////////////////////////////////////////////////////////////////////////////////////
	//////////计算纠错码部分
	int GfSum(int a, int b);
	int GfProduct(int a, int b);
	int GfDoublify(int a, int b);
	void SetEccPoly(unsigned char *g, int errorWordCount);
	void GenReedSolEcc(int index);
	void GenReedSolEccMuti(int index);
	void GenReedSolEcc144(int index);
	unsigned char Randomize253State(unsigned char codewordValue, int codewordPosition);
	void AddPadChars(int index,int padSize);
	////////////////计算纠错码部分
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////代码转换部分
	void IntToChar();
	void AddFrameData(void);
	void module(int row, int col, unsigned char chr, int bit);
	void utah(int row,int col,unsigned char chr);
	void corner1(unsigned char chr);
	void corner2(unsigned char chr);
	void corner3(unsigned char chr);
	void corner4(unsigned char chr);
	void ecc200();
	///////////////////转换部分
	/////////////////////////////////////////////////////////////////
	//////////////////
	bool OnDraw(void);
};


#endif
