#include <mbed.h>
#include "datamatrix.h"

static const short logVal[16*16] ={
	-255, 255,   1, 240,   2, 225, 241,  53,   3,  38, 226, 133, 242,  43,  54, 210,
	4, 195,  39, 114, 227, 106, 134,  28, 243, 140,  44,  23,  55, 118, 211, 234,
	5, 219, 196,  96,  40, 222, 115, 103, 228,  78, 107, 125, 135,   8,  29, 162,
	244, 186, 141, 180,  45,  99,  24,  49,  56,  13, 119, 153, 212, 199, 235,  91,
	6,  76, 220, 217, 197,  11,  97, 184,  41,  36, 223, 253, 116, 138, 104, 193,
	229,  86,  79, 171, 108, 165, 126, 145, 136,  34,   9,  74,  30,  32, 163,  84,
	245, 173, 187, 204, 142,  81, 181, 190,  46,  88, 100, 159,  25, 231,  50, 207,
	57, 147,  14,  67, 120, 128, 154, 248, 213, 167, 200,  63, 236, 110,  92, 176,
	7, 161,  77, 124, 221, 102, 218,  95, 198,  90,  12, 152,  98,  48, 185, 179,
	42, 209,  37, 132, 224,  52, 254, 239, 117, 233, 139,  22, 105,  27, 194, 113,
	230, 206,  87, 158,  80, 189, 172, 203, 109, 175, 166,  62, 127, 247, 146,  66,
	137, 192,  35, 252,  10, 183,  75, 216,  31,  83,  33,  73, 164, 144,  85, 170,
	246,  65, 174,  61, 188, 202, 205, 157, 143, 169,  82,  72, 182, 215, 191, 251,
	47, 178,  89, 151, 101,  94, 160, 123,  26, 112, 232,  21,  51, 238, 208, 131,
	58,  69, 148,  18,  15,  16,  68,  17, 121, 149, 129,  19, 155,  59, 249,  70,
	214, 250, 168,  71, 201, 156,  64,  60, 237, 130, 111,  20,  93, 122, 177, 150 };

static const short aLogVal[16*16] = {
	1,   2,   4,   8,  16,  32,  64, 128,  45,  90, 180,  69, 138,  57, 114, 228,
	229, 231, 227, 235, 251, 219, 155,  27,  54, 108, 216, 157,  23,  46,  92, 184,
	93, 186,  89, 178,  73, 146,   9,  18,  36,  72, 144,  13,  26,  52, 104, 208,
	141,  55, 110, 220, 149,   7,  14,  28,  56, 112, 224, 237, 247, 195, 171, 123,
	246, 193, 175, 115, 230, 225, 239, 243, 203, 187,  91, 182,  65, 130,  41,  82,
	164, 101, 202, 185,  95, 190,  81, 162, 105, 210, 137,  63, 126, 252, 213, 135,
	35,  70, 140,  53, 106, 212, 133,  39,  78, 156,  21,  42,  84, 168, 125, 250,
	217, 159,  19,  38,  76, 152,  29,  58, 116, 232, 253, 215, 131,  43,  86, 172,
	117, 234, 249, 223, 147,  11,  22,  44,  88, 176,  77, 154,  25,  50, 100, 200,
	189,  87, 174, 113, 226, 233, 255, 211, 139,  59, 118, 236, 245, 199, 163, 107,
	214, 129,  47,  94, 188,  85, 170, 121, 242, 201, 191,  83, 166,  97, 194, 169,
	127, 254, 209, 143,  51, 102, 204, 181,  71, 142,  49,  98, 196, 165, 103, 206,
	177,  79, 158,  17,  34,  68, 136,  61, 122, 244, 197, 167,  99, 198, 161, 111,
	222, 145,  15,  30,  60, 120, 240, 205, 183,  67, 134,  33,  66, 132,  37,  74,
	148,   5,  10,  20,  40,  80, 160, 109, 218, 153,  31,  62, 124, 248, 221, 151,
	3,   6,  12,  24,  48,  96, 192, 173, 119, 238, 241, 207, 179,  75, 150,   1 };

static const short matrixsize[24] = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 32, 36, 40, 44, 48, 52, 64, 72, 80,
	88, 96, 104,120,132,144};
static const short codesize[24] = { 3,  5,  8, 12, 18, 22, 30,  36,   44,  62, 86, 114,  144,  174, 204,
	280, 368, 456,576,696,816,1050, 1304, 1558 };
static const short unitcodesize[24] = { 3,  5,  8, 12, 18, 22, 30,  36,   44,  62, 86, 114,  144,  174, 102,
	140, 92, 114,144,174,136,175, 163, 156};
static const short errorsize[24] = { 5,  7,  10, 12, 14, 18, 20, 24, 28, 36,  42,  48,  56,  68,  84,
	112,144,192,224,272,336,408,496,620 };
static const short uniterrorsize[24] = { 5,  7,  10, 12, 14, 18, 20, 24, 28, 36,  42,  48,  56,  68,  42,
	56,36,48,56,68,56,68,62,62 };
static const short datasize[24] = {  8 ,10,12,14,16,18,20,22,24,14*2,16*2,18*2,20*2,22*2,24*2,14*4,16*4,18*4,
	20*4,22*4,24*4,18*6,20*6,22*6 };
static const short regionsize[24] = {1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,4,4,4,4,4,4,6,6,6};

Datamatrix::Datamatrix(char *code)
{
	PLOT_BUF_SIZE = (100*100/8);
	BARCODE_DATA_MAX = 256;
	BARCODE_BUF_SIZE = (BARCODE_DATA_MAX*10);

	pDataBuf = (unsigned char *)malloc(PLOT_BUF_SIZE);//DataMatrixBuf+BARCODE_BUF_SIZE;
	unsigned char *DataMatrixBuf=(unsigned char *)malloc(BARCODE_BUF_SIZE);//+PLOT_BUF_SIZE);
	memset(DataMatrixBuf,0,BARCODE_BUF_SIZE);
	memset(pDataBuf,0,PLOT_BUF_SIZE);
	need_free = false;
	code_len = strlen(code);

	unsigned char* temp1=DataMatrixBuf;
	unsigned char* temp2=temp1+sizeof(DataMatrixBuf)/2;
	if(code_len>BARCODE_DATA_MAX)goto err;
	temparray=temp1;
	p_strdatatext=temp2;
	Init(code,code_len);
	printarray = pDataBuf;
	memcpy(pDataBuf,code,code_len);
	if(OnDraw()==false)goto err;

	Width = ncol*8;
	Height = matrixsize[gIndex];
	pData = (uint8_t *)printarray;
	need_free = true;
	free(DataMatrixBuf);
	return ;
err:
	free(DataMatrixBuf);
	free(pDataBuf);
}

//编码形成码字流，然后转化为点阵，p_strdatatext无用；
//将点阵转化成放置后的码字流，temparray无用可覆盖；
//通过ReverseCharBits转化为打印机可以打印的码字流。
char Datamatrix::ReverseCharBits(char Num)
{
	Num = ((Num & 0x55) << 1) | ((Num >> 1) & 0x55);
	Num = ((Num & 0x33) << 2 )| ((Num >> 2) & 0x33);
	Num = ((Num & 0x0F) << 4 )| ((Num >> 4) & 0x0F);
	return Num;
}
	
////////////////////////初始化
void Datamatrix::Init(char* buf,int len)
{
	nrow=0;
	ncol=0;
	gIndex=0;
	m_padsize=0;
	m_codenumber=0;
	printarray=(unsigned char*) buf;
	m_textlen=len;
}

///////////////////编码部分
unsigned char Datamatrix::Randomize255State(unsigned char codewordValue, int codewordPosition)
{
	int pseudoRandom;
	int tmp;
	pseudoRandom = ((149 * codewordPosition) % 255) + 1;
	tmp = codewordValue + pseudoRandom;
	return (tmp <= 256) ? tmp : tmp - 256;
}

bool Datamatrix::EncodeBase256(int begin,int textlen,int precodenumber)
{
	int i;
	int position;
	int d1,d2;
	d1=textlen;
	p_strdatatext[precodenumber]=(char)231;
	if(textlen<250)
	{
		p_strdatatext[precodenumber+1]=Randomize255State(d1,precodenumber+1+1);
		for(i=begin;i<begin+textlen;i++)
		{
			position=precodenumber+2+i-begin;
			p_strdatatext[position]=Randomize255State(printarray[i],position+1);
		}
		m_codenumber+=2+textlen;
	}
	else if(textlen>=250&&textlen<=1555)
	{
		d1=(textlen/250)+249;
		p_strdatatext[precodenumber+1]=Randomize255State(d1,precodenumber+1+1);
		d2=textlen%250;
		p_strdatatext[precodenumber+2]=Randomize255State(d2,precodenumber+2+1);
		for(i=begin;i<begin+textlen;i++)
		{
			position=precodenumber+3+i-begin;
			p_strdatatext[position]=Randomize255State(printarray[i],position+1);
		}
		m_codenumber+=3+textlen;
	}
	else return false;
	return true;

}

bool Datamatrix::Encode()
{
	if(EncodeBase256(0,m_textlen,0)==false)return false;
	return true;
}

/////////////////////编码部分
///////////////////////////////////////////////////////////////////////////////////////
//////////////////////选择放置矩阵
bool Datamatrix::ChooseIndex(int codenumber)
{
	int i=0;
	if(codenumber>BARCODE_DATA_MAX) return false;
	while(1)
	{
		if(codenumber<=codesize[i])
		{
			gIndex=i;
			m_padsize=codesize[i]-codenumber;
			break;
		}
		i++;
	}
	ncol=nrow=datasize[gIndex];
	m_codenumber+=m_padsize;
	return true;
}

//////////////////////选择放置矩阵
///////////////////////////////////////////////////////////////////////////////////////
//////////计算纠错码部分
int Datamatrix::GfSum(int a, int b)
{
	return a ^ b;
}

int Datamatrix::GfProduct(int a, int b)
{
	if(a == 0 || b == 0)
		return 0;
	else
		return aLogVal[(logVal[a] + logVal[b]) % 255];
}

int Datamatrix::GfDoublify(int a, int b)
{
	if(a == 0)
		return 0;
	else if(b == 0)
		return a;
	else
		return aLogVal[(logVal[a] + b) % 255];
}

void Datamatrix::SetEccPoly(unsigned char *g, int errorWordCount)
{
	int i, j;

	memset(g, 0x01, sizeof(unsigned char) * (errorWordCount + 1));

	for(i = 1; i <= errorWordCount; i++)
	{
		for(j = i - 1; j >= 0; j--)
		{
			g[j] = GfDoublify(g[j], i);     // g[j] *= 2**i
			if(j > 0)
				g[j] = GfSum(g[j], g[j-1]);  // g[j] += g[j-1]
		}
	}
}

void Datamatrix::GenReedSolEcc(int index)
{
	int i, j, val;
	int dataLength = codesize[index];
	int errorWordCount = errorsize[index];
	int totalLength = dataLength + errorWordCount;
	//unsigned char g[69], b[68];
	unsigned char g[69], b[68];
	SetEccPoly(g, errorWordCount);
	memset(b, 0x00, sizeof(b));
	for(i = 0; i < totalLength; i++)
	{
		if(i < dataLength)
		{
			val = GfSum(b[errorWordCount-1], p_strdatatext[i]);
			for(j = errorWordCount - 1; j > 0; j--)
			{
				b[j] = GfSum(b[j-1], GfProduct(g[j], val));
			}
			b[0] = GfProduct(g[0], val);
		}
		else
		{
			p_strdatatext[i] =b[totalLength-i-1];
		}
	}
	for(i=0;i<totalLength;i++)
	{
		printarray[i]=p_strdatatext[i];
	}
	m_codenumber+=errorsize[index];
}

void Datamatrix::GenReedSolEccMuti(int index)
{
	int i, j,k,val;
	int dataLength = codesize[index];
	int unitdataLength=unitcodesize[index];
	int errorWordCount =errorsize[index];
	int uniterrorWordCount=uniterrorsize[index];
	int unit=errorWordCount/uniterrorWordCount;
	int unittotalLength = unitdataLength + uniterrorWordCount;
	int totalLength = dataLength + errorWordCount;
	unsigned char g[69], b[68];
	//unsigned char g[621], b[620];
	for(k=0;k<unit;k++)
	{
		for(i=0;i<unitdataLength;i++)
		{
			temparray[i]=p_strdatatext[k+i*unit];
		}
		SetEccPoly(g, uniterrorWordCount);
		memset(b, 0x00, sizeof(b));
		for(i = 0; i < unittotalLength; i++)
		{
			if(i < unitdataLength)
			{
				val = GfSum(b[uniterrorWordCount-1], temparray[i]);

				for(j = uniterrorWordCount - 1; j > 0; j--)
				{
					b[j] = GfSum(b[j-1], GfProduct(g[j], val));
				}
				b[0] = GfProduct(g[0], val);
			}

			else
			{
				temparray[i] =b[unittotalLength-i-1];
			}
		}
		for(i=0;i<uniterrorWordCount;i++)
		{
			p_strdatatext[dataLength+k+i*unit]=temparray[i+unitdataLength];
		}
	}
	for(i=0;i<totalLength;i++)
	{
		printarray[i]=p_strdatatext[i];
	}
	m_codenumber+=errorsize[index];
}

void Datamatrix::GenReedSolEcc144(int index)
{
	int i, j,k,val;
	int dataLength = codesize[index];
	int unitdataLength=unitcodesize[index];
	int errorWordCount =errorsize[index];
	int uniterrorWordCount=uniterrorsize[index];
	int unit=errorWordCount/uniterrorWordCount;
	int unittotalLength = unitdataLength + uniterrorWordCount;
	int totalLength=dataLength + errorWordCount;
	unsigned char g[69], b[68];
	//unsigned char g[621], b[620];
	for(k=0;k<8;k++)
	{
		for(i=0;i<156;i++)
		{
			temparray[i]=p_strdatatext[k+i*unit];
		}
		SetEccPoly(g, uniterrorWordCount);
		memset(b, 0x00, sizeof(b));
		for(i = 0; i < unittotalLength; i++)
		{
			if(i < unitdataLength)
			{
				val = GfSum(b[uniterrorWordCount-1], temparray[i]);

				for(j = uniterrorWordCount - 1; j > 0; j--)
				{
					b[j] = GfSum(b[j-1], GfProduct(g[j], val));
				}
				b[0] = GfProduct(g[0], val);
			}

			else
			{
				temparray[i] =b[unittotalLength-i-1];
			}
		}
		for(i=0;i<uniterrorWordCount;i++)
		{
			p_strdatatext[dataLength+k+i*unit]=temparray[i+unitdataLength];
		}
	}
	for(k=8;k<10;k++)
	{
		for(i=0;i<155;i++)
		{
			temparray[i]=p_strdatatext[k+i*unit];
		}

		SetEccPoly(g, uniterrorWordCount);
		memset(b, 0x00, sizeof(b));

		for(i = 0; i < unittotalLength-1; i++)
		{
			if(i < unitdataLength-1)
			{
				val = GfSum(b[uniterrorWordCount-1], temparray[i]);

				for(j = uniterrorWordCount - 1; j > 0; j--)
				{
					b[j] = GfSum(b[j-1], GfProduct(g[j], val));
				}
				b[0] = GfProduct(g[0], val);
			}

			else
			{
				temparray[i] =b[unittotalLength-i-1];
			}
		}
		for(i=0;i<uniterrorWordCount;i++)
		{
			p_strdatatext[dataLength+k+i*unit]=temparray[i+unitdataLength];
		}
	}
	for(i=0;i<totalLength;i++)
	{
		printarray[i]=p_strdatatext[i];
	}

	m_codenumber+=errorsize[index];
}

unsigned char Datamatrix::Randomize253State(unsigned char codewordValue, int codewordPosition)
{
	int pseudoRandom;
	int tmp;
	pseudoRandom = ((149 * codewordPosition) % 253) + 1;
	tmp = codewordValue + pseudoRandom;
	return (tmp <= 254) ? tmp : tmp - 254;
}

void Datamatrix::AddPadChars(int index,int padSize)
{
	int i;
	int dataSize;
	int padBegin;
	dataSize = codesize[index];
	padBegin = dataSize - padSize;
	for(i = padBegin; i < dataSize; i++)
	{
		p_strdatatext[i] = (i == padBegin) ? 129 : Randomize253State(129, i + 1);
	}
}
////////////////计算纠错码部分
////////////////////////////////////////////////////////////////////////////////////////////
////////////////代码转换部分
void Datamatrix::IntToChar()
{
	int i,j;
	int col=(ncol-1)/8+1;
	//	int row=nrow;
	//memset(chrarray,0,1000);
	memset(printarray,0,PLOT_BUF_SIZE);
	for(i=0;i<nrow;i++)
	{
		for(j=0;j<ncol;j++)
		{
			printarray[i*col+j/8]|=(temparray[i*ncol+j]<<(7-j%8));
		}
	}
	ncol=col;

	for(i=0;i<nrow;i++)
	{
		for(j=0;j<ncol;j++)
		{
			printarray[i*ncol+j]=ReverseCharBits(printarray[i*ncol+j]);
		}
	}

}

void Datamatrix::AddFrameData(void)
{
	int i,j;
	int row=matrixsize[gIndex];
	int col=matrixsize[gIndex];
	//	int unitrow=datasize[index];
	int unitcol=datasize[gIndex];
	int regionnum=regionsize[gIndex];
	if(1==regionnum)
	{
		for(i=row-1;i>=0;i--)
		{
			for(j=col-1;j>=0;j--)
			{
				if(i==(col-1)) temparray[i*(col)+j]=1;
				else if(i==0) temparray[i*(col)+j]=(j+1)%2;
				else
				{
					if(j==0) temparray[i*(col)+j]=1;
					else if(j==col-1) temparray[i*(col)+j]=i%2;
					else temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-1];
				}
			}
		}

	}
	else if(2==regionnum)
	{
		for(i=row-1;i>=0;i--)
		{
			for(j=col-1;j>=0;j--)
			{
				if((i==row-1)||(i==row-1-row/regionnum)) temparray[i*(col)+j]=1;
				else if((i==0)||(i==row/regionnum)) temparray[i*(col)+j]=(j+1)%2;
				else
				{
					if((j==0)||(j==col/regionnum)) temparray[i*(col)+j]=1;
					else if((j==col-1)||(j==col-1-row/regionnum)) temparray[i*(col)+j]=i%2;
					else
					{
						if(i<row-1-row/regionnum)
						{
							if(j<unitcol/2+2)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-1];
							else if(j<(unitcol/2+2)*2)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-3];
						}
						else if(i<row-1)
						{
							if(j<unitcol/2+2)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-1];
							else if(j<(unitcol/2+2)*2)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-3];
						}
					}
				}
			}
		}
	}
	else if(4==regionnum)
	{
		for(i=row-1;i>=0;i--)
		{
			for(j=col-1;j>=0;j--)
			{
				if((i==row-1)||(i==row-1-row/regionnum)||(i==row-1-2*row/regionnum)||(i==row-1-3*row/regionnum))
					temparray[i*(col)+j]=1;
				else if((i==0)||(i==row/regionnum)||(i==2*row/regionnum)||(i==3*row/regionnum)) temparray[i*(col)+j]=(j+1)%2;
				else
				{
					if((j==col-1)||(j==col-1-row/regionnum)||(j==col-1-2*row/regionnum)||(j==col-1-3*row/regionnum)) temparray[i*(col)+j]=i%2;
					else if((j==0)||(j==row/regionnum)||(j==2*row/regionnum)||(j==3*row/regionnum)) temparray[i*(col)+j]=1;
					else
					{
						if(i<col-1-3*row/regionnum)
						{
							if(j<unitcol/4+2)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-1];
							else if(j<(unitcol/4+2)*2)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-3];
							else if(j<(unitcol/4+2)*3)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-5];
							else if(j<(unitcol/4+2)*4)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-7];
						}
						else if(i<col-1-2*row/regionnum)
						{
							if(j<unitcol/4+2)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-1];
							else if(j<(unitcol/4+2)*2)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-3];
							else if(j<(unitcol/4+2)*3)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-5];
							else if(j<(unitcol/4+2)*4)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-7];
						}
						else if(i<col-1-row/regionnum)
						{
							if(j<unitcol/4+2)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-1];
							else if(j<(unitcol/4+2)*2)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-3];
							else if(j<(unitcol/4+2)*3)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-5];
							else if(j<(unitcol/4+2)*4)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-7];
						}
						else if(i<col-1)
						{
							if(j<unitcol/4+2)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-1];
							else if(j<(unitcol/4+2)*2)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-3];
							else if(j<(unitcol/4+2)*3)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-5];
							else if(j<(unitcol/4+2)*4)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-7];
						}
					}
				}
			}
		}
	}
	else if(6==regionnum)
	{
		for(i=row-1;i>=0;i--)
		{
			for(j=col-1;j>=0;j--)
			{
				if((i==col-1)||(i==col-1-row/regionnum)||(i==col-1-2*row/regionnum)||(i==col-1-3*row/regionnum)||(i==col-1-5*row/regionnum)||(i==col-1-4*row/regionnum)) temparray[i*(col)+j]=1;
				else if((i==0)||(i==row/regionnum)||(i==2*row/regionnum)||(i==3*row/regionnum)||(i==4*row/regionnum)||(i==5*row/regionnum)) temparray[i*(col)+j]=(j+1)%2;
				else
				{
					if((j==col-1)||(j==col-1-row/regionnum)||(j==col-1-2*row/regionnum)||(j==col-1-3*row/regionnum)||(j==col-1-4*row/regionnum)||(j==col-1-5*row/regionnum)) temparray[i*(col)+j]=i%2;
					else if((j==0)||(j==row/regionnum)||(j==2*row/regionnum)||(j==3*row/regionnum)||(j==4*row/regionnum)||(j==5*row/regionnum)) temparray[i*(col)+j]=1;
					else
					{
						if(i<col-1-5*row/regionnum)
						{
							if(j<unitcol/6+2)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-1];
							else if(j<(unitcol/6+2)*2)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-3];
							else if(j<(unitcol/6+2)*3)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-5];
							else if(j<(unitcol/6+2)*4)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-7];
							else if(j<(unitcol/6+2)*5)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-9];
							else if(j<(unitcol/6+2)*6)
								temparray[i*(col)+j]=temparray[(i-1)*datasize[gIndex]+j-11];
						}
						else if(i<col-1-4*row/regionnum)
						{
							if(j<unitcol/6+2)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-1];
							else if(j<(unitcol/6+2)*2)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-3];
							else if(j<(unitcol/6+2)*3)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-5];
							else if(j<(unitcol/6+2)*4)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-7];
							else if(j<(unitcol/6+2)*5)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-9];
							else if(j<(unitcol/6+2)*6)
								temparray[i*(col)+j]=temparray[(i-3)*datasize[gIndex]+j-11];
						}
						else if(i<col-1-3*row/regionnum)
						{
							if(j<unitcol/6+2)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-1];
							else if(j<(unitcol/6+2)*2)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-3];
							else if(j<(unitcol/6+2)*3)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-5];
							else if(j<(unitcol/6+2)*4)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-7];
							else if(j<(unitcol/6+2)*5)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-9];
							else if(j<(unitcol/6+2)*6)
								temparray[i*(col)+j]=temparray[(i-5)*datasize[gIndex]+j-11];
						}
						else if(i<col-1-2*row/regionnum)
						{
							if(j<unitcol/6+2)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-1];
							else if(j<(unitcol/6+2)*2)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-3];
							else if(j<(unitcol/6+2)*3)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-5];
							else if(j<(unitcol/6+2)*4)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-7];
							else if(j<(unitcol/6+2)*5)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-9];
							else if(j<(unitcol/6+2)*6)
								temparray[i*(col)+j]=temparray[(i-7)*datasize[gIndex]+j-11];
						}
						else if(i<col-1-row/regionnum)
						{
							if(j<unitcol/6+2)
								temparray[i*(col)+j]=temparray[(i-9)*datasize[gIndex]+j-1];
							else if(j<(unitcol/6+2)*2)
								temparray[i*(col)+j]=temparray[(i-9)*datasize[gIndex]+j-3];
							else if(j<(unitcol/6+2)*3)
								temparray[i*(col)+j]=temparray[(i-9)*datasize[gIndex]+j-5];
							else if(j<(unitcol/6+2)*4)
								temparray[i*(col)+j]=temparray[(i-9)*datasize[gIndex]+j-7];
							else if(j<(unitcol/6+2)*5)
								temparray[i*(col)+j]=temparray[(i-9)*datasize[gIndex]+j-9];
							else if(j<(unitcol/6+2)*6)
								temparray[i*(col)+j]=temparray[(i-9)*datasize[gIndex]+j-11];
						}
						else if(i<col-1)
						{
							if(j<unitcol/6+2)
								temparray[i*(col)+j]=temparray[(i-11)*datasize[gIndex]+j-1];
							else if(j<(unitcol/6+2)*2)
								temparray[i*(col)+j]=temparray[(i-11)*datasize[gIndex]+j-3];
							else if(j<(unitcol/6+2)*3)
								temparray[i*(col)+j]=temparray[(i-11)*datasize[gIndex]+j-5];
							else if(j<(unitcol/6+2)*4)
								temparray[i*(col)+j]=temparray[(i-11)*datasize[gIndex]+j-7];
							else if(j<(unitcol/6+2)*5)
								temparray[i*(col)+j]=temparray[(i-11)*datasize[gIndex]+j-9];
							else if(j<(unitcol/6+2)*6)
								temparray[i*(col)+j]=temparray[(i-11)*datasize[gIndex]+j-11];
						}
					}
				}
			}
		}
	}
	nrow=row;
	ncol=col;
}

void Datamatrix::module(int row, int col, unsigned char chr, int bit)
{
	if(row<0){row+=nrow;col+=4-((nrow+4)%8);}
	if(col<0){col+=ncol;row+=4-((ncol+4)%8);}
	temparray[row*ncol+col]=chr&(0x01<<(8-bit));
	if(temparray[row*ncol+col]!=0)
		temparray[row*ncol+col]=1;
}

void Datamatrix::utah(int row,int col,unsigned char chr)
{
	module(row-2,col-2,chr,1);
	module(row-2,col-1,chr,2);
	module(row-1,col-2,chr,3);
	module(row-1,col-1,chr,4);
	module(row-1,col,chr,5);
	module(row,col-2,chr,6);
	module(row,col-1,chr,7);
	module(row,col,chr,8);
}

void Datamatrix::corner1(unsigned char chr)
{
	module(nrow-1,0,chr,1);
	module(nrow-1,1,chr,2);
	module(nrow-1,2,chr,3);
	module(0,ncol-2,chr,4);
	module(0,ncol-1,chr,5);
	module(1,ncol-1,chr,6);
	module(2,ncol-1,chr,7);
	module(3,ncol-1,chr,8);
}

void Datamatrix::corner2(unsigned char chr)
{
	module(nrow-3,0,chr,1);
	module(nrow-2,0,chr,2);
	module(nrow-1,0,chr,3);
	module(0,ncol-4,chr,4);
	module(0,ncol-3,chr,5);
	module(0,ncol-2,chr,6);
	module(0,ncol-1,chr,7);
	module(1,ncol-1,chr,8);
}

void Datamatrix::corner3(unsigned char chr)
{
	module(nrow-3,0,chr,1);
	module(nrow-2,0,chr,2);
	module(nrow-1,0,chr,3);
	module(0,ncol-2,chr,4);
	module(0,ncol-1,chr,5);
	module(1,ncol-1,chr,6);
	module(2,ncol-1,chr,7);
	module(3,ncol-1,chr,8);
}

void Datamatrix::corner4(unsigned char chr)
{
	module(nrow-1,0,chr,1);
	module(nrow-1,ncol-1,chr,2);
	module(0,ncol-3,chr,3);
	module(0,ncol-2,chr,4);
	module(0,ncol-1,chr,5);
	module(1,ncol-3,chr,6);
	module(1,ncol-2,chr,7);
	module(1,ncol-1,chr,8);
}

void Datamatrix::ecc200()
{
	int row,col,chr;

	for(row=0;row<nrow;row++)
	{
		for(col=0;col<ncol;col++)
		{
			temparray[row*ncol+col]=2;
		}
	}
	chr=0;row=4;col=0;
	do
	{
		if((row==nrow)&&(col==0))corner1(printarray[chr++]);
		if((row==nrow-2)&&(col==0)&&(ncol%4))corner2(printarray[chr++]);
		if((row==nrow-2)&&(col==0)&&(ncol%8==4))corner3(printarray[chr++]);
		if((row==nrow+4)&&(col==2)&&(!(ncol%8)))corner4(printarray[chr++]);
		do
		{
			if((row<nrow)&&(col>=0)&&(temparray[row*ncol+col]==2))
				utah(row,col,printarray[chr++]);
			row-=2;col+=2;
		}while((row>=0)&&(col<ncol));
		row+=1;col+=3;
		do
		{
			if((row>=0)&&(col<ncol)&&(temparray[row*ncol+col]==2))
				utah(row,col,printarray[chr++]);
			row+=2;col-=2;
		}while((row<nrow)&&(col>=0));
		row+=3;col+=1;
	}while((row<nrow)||(col<ncol));
	if(temparray[row*ncol+col]==2)
	{
		temparray[nrow*ncol-1]=temparray[nrow*(ncol-1)-2]=1;
	}
}

///////////////////转换部分
/////////////////////////////////////////////////////////////////
//////////////////
bool Datamatrix::OnDraw(void)
{
	if(Encode()==false) return false;
	ChooseIndex(m_codenumber);
	AddPadChars(gIndex,m_padsize);
	if(gIndex<=13&&gIndex>0)
		GenReedSolEcc(gIndex);
	else if(gIndex<=22)
		GenReedSolEccMuti(gIndex);
	else if(gIndex==23)
		GenReedSolEcc144(gIndex);
	ecc200();
	AddFrameData();
	IntToChar();
	return true;
}


