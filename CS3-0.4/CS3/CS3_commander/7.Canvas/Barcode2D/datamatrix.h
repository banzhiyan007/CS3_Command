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
	//������������ı���Ϣ�����ı����������������temparray���ÿɸ��ǣ�
	//���ת��Ϊ���ݵ���
	//unsigned char *printchar; //size=18*144(2596)
	unsigned char *p_strdatatext;//size=18*144(2596)
	unsigned char *pDataBuf ;
	int code_len;
public :
	Datamatrix(char *code);
private:
	//�����γ���������Ȼ��ת��Ϊ����p_strdatatext���ã�
	//������ת���ɷ��ú����������temparray���ÿɸ��ǣ�
	//ͨ��ReverseCharBitsת��Ϊ��ӡ�����Դ�ӡ����������
	char ReverseCharBits(char Num);
	////////////////////////��ʼ��
	void Init(char* buf,int len);
	///////////////////���벿��
	unsigned char Randomize255State(unsigned char codewordValue, int codewordPosition);
	bool EncodeBase256(int begin,int textlen,int precodenumber);
	bool Encode();
	/////////////////////���벿��
	///////////////////////////////////////////////////////////////////////////////////////
	//////////////////////ѡ����þ���
	bool ChooseIndex(int codenumber);
	//////////////////////ѡ����þ���
	///////////////////////////////////////////////////////////////////////////////////////
	//////////��������벿��
	int GfSum(int a, int b);
	int GfProduct(int a, int b);
	int GfDoublify(int a, int b);
	void SetEccPoly(unsigned char *g, int errorWordCount);
	void GenReedSolEcc(int index);
	void GenReedSolEccMuti(int index);
	void GenReedSolEcc144(int index);
	unsigned char Randomize253State(unsigned char codewordValue, int codewordPosition);
	void AddPadChars(int index,int padSize);
	////////////////��������벿��
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////����ת������
	void IntToChar();
	void AddFrameData(void);
	void module(int row, int col, unsigned char chr, int bit);
	void utah(int row,int col,unsigned char chr);
	void corner1(unsigned char chr);
	void corner2(unsigned char chr);
	void corner3(unsigned char chr);
	void corner4(unsigned char chr);
	void ecc200();
	///////////////////ת������
	/////////////////////////////////////////////////////////////////
	//////////////////
	bool OnDraw(void);
};


#endif
