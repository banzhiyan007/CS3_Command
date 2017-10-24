#include <mbed.h>
#include "qrcode.h"

static const unsigned short QR_FORMATS[8][4]=
{
	{0,0,0,0},				//mask:000
	{0,0,0,0},				//mask:001
	{0,0,0,0},				//mask:010
	{0x789d,0x5b4b,0x2a02,0x18d2},				//mask:011
	{0,0,0,0},				//mask:100
	{0,0,0,0},				//mask:101
	{0,0,0,0},				//mask:110
	{0,0,0,0}				//mask:111
};

static const unsigned long  QR_VERSION_INFO[40 - 6] = {
	0x07c94, 0x085bc, 0x09a99, 0x0a4d3, 0x0bbf6, 0x0c762, 0x0d847, 0x0e60d,
	0x0f928, 0x10b78, 0x1145d, 0x12a17, 0x13532, 0x149a6, 0x15683, 0x168c9,
	0x177ec, 0x18ec4, 0x191e1, 0x1afab, 0x1b08e, 0x1cc1a, 0x1d33f, 0x1ed75,
	0x1f250, 0x209d5, 0x216f0, 0x228ba, 0x2379f, 0x24b0b, 0x2542e, 0x26a64,
	0x27541, 0x28c69
};

static const unsigned short QR_DATA_SIZE[40][4]=
{
	{19,16,13,9},{34,28,22,16},{55,44,34,26},{80,64,48,36},{108,86,62,46},
	{136,108,76,60},{156,124,88,66},{194,154,110,86},{232,182,132,100},{274,216,154,122},
	{324,254,180,140},{370,290,206,158},{428,334,244,180},{461,365,261,197},{523,415,295,223},
	{589,453,325,253},{647,507,367,283},{721,563,397,313},{795,627,445,341},{861,669,485,385},
	{932,714,512,406},{1006,782,568,442},{1094,860,614,464},{1174,914,664,514},{1276,1000,718,538},
	{1370,1062,754,596},{1468,1128,808,628},{1531,1193,871,661},{1631,1267,911,701},{1735,1373,985,745},
	{1843,1455,1033,793},{1955,1541,1115,845},{2071,1631,1171,901},{2191,1725,1231,961},{2306,1812,1286,986},
	{2434,1914,1354,1054},{2566,1992,1426,1096},{2702,2102,1502,1142},{2812,2216,1582,1222},{2956,2334,1666,1276},
};

static const unsigned short QR_TOTAL_SIZE[40]=
{
	26,44,70,100,134,172,196,242,292,346,404,466,532,581,655,733,815,901,991,1085,
	1156,1258,1364,1474,1588,1706,1828,1921,2051,2185,2323,2465,2611,2761,2876,3034,3196,3362,3532,3706,
};

static const unsigned char QR_DATA_BLOCK_COUNT[40][4]=
{
	{1,1,1,1},{1,1,1,1},{1,1,2,2},{1,2,2,4},{1,2,4,4},
	{2,4,4,4},{2,4,6,5},{2,4,6,6},{2,5,8,8},{4,5,8,8},
	{4,5,8,11},{4,8,10,11},{4,9,12,16},{4,9,16,16},{6,10,12,18},
	{6,10,17,16},{6,11,16,19},{6,13,18,21},{7,14,21,25},{8,16,20,25},
	{8,17,23,25},{9,17,23,34},{9,18,25,30},{10,20,27,32},{12,21,29,35},
	{12,23,34,37},{12,25,34,40},{13,26,35,42},{14,28,38,45},{15,29,40,48},
	{16,31,43,51},{17,33,45,54},{18,35,48,57},{19,37,51,60},{19,38,53,63},
	{20,40,56,66},{21,43,59,70},{22,45,62,74},{24,47,65,77},{25,49,68,81},
};

//galois========================================================================
static const unsigned char exp_table[512]=
{
	1,   2,   4,   8,  16,  32,  64, 128,  29,  58, 116, 232, 205, 135,  19,  38,
	76, 152,  45,  90, 180, 117, 234, 201, 143,   3,   6,  12,  24,  48,  96, 192,
	157,  39,  78, 156,  37,  74, 148,  53, 106, 212, 181, 119, 238, 193, 159,  35,
	70, 140,   5,  10,  20,  40,  80, 160,  93, 186, 105, 210, 185, 111, 222, 161,
	95, 190,  97, 194, 153,  47,  94, 188, 101, 202, 137,  15,  30,  60, 120, 240,
	253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163,  91, 182, 113, 226,
	217, 175,  67, 134,  17,  34,  68, 136,  13,  26,  52, 104, 208, 189, 103, 206,
	129,  31,  62, 124, 248, 237, 199, 147,  59, 118, 236, 197, 151,  51, 102, 204,
	133,  23,  46,  92, 184, 109, 218, 169,  79, 158,  33,  66, 132,  21,  42,  84,
	168,  77, 154,  41,  82, 164,  85, 170,  73, 146,  57, 114, 228, 213, 183, 115,
	230, 209, 191,  99, 198, 145,  63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
	227, 219, 171,  75, 150,  49,  98, 196, 149,  55, 110, 220, 165,  87, 174,  65,
	130,  25,  50, 100, 200, 141,   7,  14,  28,  56, 112, 224, 221, 167,  83, 166,
	81, 162,  89, 178, 121, 242, 249, 239, 195, 155,  43,  86, 172,  69, 138,   9,
	18,  36,  72, 144,  61, 122, 244, 245, 247, 243, 251, 235, 203, 139,  11,  22,
	44,  88, 176, 125, 250, 233, 207, 131,  27,  54, 108, 216, 173,  71, 142,   1,
	2,   4,   8,  16,  32,  64, 128,  29,  58, 116, 232, 205, 135,  19,  38,  76,
	152,  45,  90, 180, 117, 234, 201, 143,   3,   6,  12,  24,  48,  96, 192, 157,
	39,  78, 156,  37,  74, 148,  53, 106, 212, 181, 119, 238, 193, 159,  35,  70,
	140,   5,  10,  20,  40,  80, 160,  93, 186, 105, 210, 185, 111, 222, 161,  95,
	190,  97, 194, 153,  47,  94, 188, 101, 202, 137,  15,  30,  60, 120, 240, 253,
	231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163,  91, 182, 113, 226, 217,
	175,  67, 134,  17,  34,  68, 136,  13,  26,  52, 104, 208, 189, 103, 206, 129,
	31,  62, 124, 248, 237, 199, 147,  59, 118, 236, 197, 151,  51, 102, 204, 133,
	23,  46,  92, 184, 109, 218, 169,  79, 158,  33,  66, 132,  21,  42,  84, 168,
	77, 154,  41,  82, 164,  85, 170,  73, 146,  57, 114, 228, 213, 183, 115, 230,
	209, 191,  99, 198, 145,  63, 126, 252, 229, 215, 179, 123, 246, 241, 255, 227,
	219, 171,  75, 150,  49,  98, 196, 149,  55, 110, 220, 165,  87, 174,  65, 130,
	25,  50, 100, 200, 141,   7,  14,  28,  56, 112, 224, 221, 167,  83, 166,  81,
	162,  89, 178, 121, 242, 249, 239, 195, 155,  43,  86, 172,  69, 138,   9,  18,
	36,  72, 144,  61, 122, 244, 245, 247, 243, 251, 235, 203, 139,  11,  22,  44,
	88, 176, 125, 250, 233, 207, 131,  27,  54, 108, 216, 173,  71, 142,   1,   0
};

static const unsigned char log_table[256]=
{
	0,   0,   1,  25,   2,  50,  26, 198,   3, 223,  51, 238,  27, 104, 199,  75,
	4, 100, 224,  14,  52, 141, 239, 129,  28, 193, 105, 248, 200,   8,  76, 113,
	5, 138, 101,  47, 225,  36,  15,  33,  53, 147, 142, 218, 240,  18, 130,  69,
	29, 181, 194, 125, 106,  39, 249, 185, 201, 154,   9, 120,  77, 228, 114, 166,
	6, 191, 139,  98, 102, 221,  48, 253, 226, 152,  37, 179,  16, 145,  34, 136,
	54, 208, 148, 206, 143, 150, 219, 189, 241, 210,  19,  92, 131,  56,  70,  64,
	30,  66, 182, 163, 195,  72, 126, 110, 107,  58,  40,  84, 250, 133, 186,  61,
	202,  94, 155, 159,  10,  21, 121,  43,  78, 212, 229, 172, 115, 243, 167,  87,
	7, 112, 192, 247, 140, 128,  99,  13, 103,  74, 222, 237,  49, 197, 254,  24,
	227, 165, 153, 119,  38, 184, 180, 124,  17,  68, 146, 217,  35,  32, 137,  46,
	55,  63, 209,  91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190,  97,
	242,  86, 211, 171,  20,  42,  93, 158, 132,  60,  57,  83,  71, 109,  65, 162,
	31,  45,  67, 216, 183, 123, 164, 118, 196,  23,  73, 236, 127,  12, 111, 246,
	108, 161,  59,  82,  41, 157,  85, 170, 251,  96, 134, 177, 187, 204,  62,  90,
	203,  89,  95, 176, 156, 169, 160,  81,  11, 245,  22, 235, 122, 117,  44, 215,
	79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168,  80,  88, 175
};
//genPoly_QRCode================================================================
//QRCode采用的生成多项式,按照规格书上来,与自动生成的顺序相反
static const unsigned char genPoly7[7+1]={1,87,229,146,149,238,102,21};
static const unsigned char genPoly10[10+1]={1,251,67,46,61,118,70,64,94,32,45};
static const unsigned char genPoly13[13+1]={1,74,152,176,100,86,100,106,104,130,218,206,140,78};
static const unsigned char genPoly15[15+1]={1,8,183,61,91,202,37,51,58,58,237,140,124,5,99,105};
static const unsigned char genPoly16[16+1]={1,120,104,107,109,102,161,76,3,91,191,147,169,182,194,225,120};
static const unsigned char genPoly17[17+1]={1,43,139,206,78,43,239,123,206,214,147,24,99,150,39,243,163,136};
static const unsigned char genPoly18[18+1]={1,215,234,158,94,184,97,118,170,79,187,152,148,252,179,5,98,96,153};
static const unsigned char genPoly20[20+1]={1,17,60,79,50,61,163,26,187,202,180,221,225,83,239,156,164,212,212,188,190};
static const unsigned char genPoly22[22+1]={1,210,171,247,242,93,230,14,109,221,53,200,74,8,172,98,80,219,134,160,105,165,231};
static const unsigned char genPoly24[24+1]={1,229,121,135,48,211,117,251,126,159,180,169,152,192,226,228,218,111,0,117,232,87,96,227,21};
static const unsigned char genPoly26[26+1]={1,173,125,158,2,103,182,118,17,145,201,111,28,165,53,161,21,245,142,13,102,48,227,153,145,218,70};
static const unsigned char genPoly28[28+1]={1,168,223,200,104,224,234,108,180,110,190,195,147,205,27,232,201,21,43,245,87,42,195,212,119,242,37,9,123};
static const unsigned char genPoly30[30+1]={1,41,173,145,152,216,31,179,182,50,48,110,86,239,96,222,125,42,173,226,193,224,130,156,37,251,216,238,40,192,180};

QRCode::QRCode(int qrcodeVer,int qrcodeRank,char *code)
{
	NPAR_MAX = 30;
	plot_mask_type=3;
	int code_len = strlen(code);

	qrcode_calc(qrcodeVer,qrcodeRank,code,code_len);
	qr_line_bytes = (qr_size-1)/8+1;
	int plot_buf_size=qr_line_bytes*qr_size;
	int datasize=QR_TOTAL_SIZE[qr_ver-1]+8+16;

	char *plotbuf=(char*)malloc(plot_buf_size+datasize);
	memset(plotbuf,0,plot_buf_size+datasize);
	memcpy(plotbuf+plot_buf_size+8,code,code_len);

	qrcode_encode(qr_ver,qr_rank,plotbuf+plot_buf_size+8,code_len);
	ptrPlot = (unsigned char *)plotbuf;
	qrcode_make_border(1);
	//采取固定的011掩模,如果采用掩模评价方式,可能导致运算速度过慢
	qr_mask=3;
	qrcode_output_data(qr_mask);
	QR_FORMAT=QR_FORMATS[qr_mask][qr_rank-1];
	qrcode_make_border(0);
	Width = qr_size;
	Height = qr_size;
	pData = (uint8_t *)plotbuf;
	need_free = true;
}

void QRCode::reedsolomon_qrcode(unsigned char *databuf,int datalen,unsigned char *parbuf,int parlen)
{
    unsigned char dbyte;
    unsigned char genPoly[NPAR_MAX+1],LFSR[NPAR_MAX],*pG;
    int i,j;

    if(parlen>NPAR_MAX)return;

    switch(parlen)
    {
        case	7:pG=(unsigned char *)genPoly7;break;
        case	10:pG=(unsigned char *)genPoly10;break;
        case	13:pG=(unsigned char *)genPoly13;break;
        case	15:pG=(unsigned char *)genPoly15;break;
        case	16:pG=(unsigned char *)genPoly16;break;
        case	17:pG=(unsigned char *)genPoly17;break;
        case	18:pG=(unsigned char *)genPoly18;break;
        case	20:pG=(unsigned char *)genPoly20;break;
        case	22:pG=(unsigned char *)genPoly22;break;
        case	24:pG=(unsigned char *)genPoly24;break;
        case	26:pG=(unsigned char *)genPoly26;break;
        case	28:pG=(unsigned char *)genPoly28;break;
        case	30:pG=(unsigned char *)genPoly30;break;
        default:return;
    }
    for(i=0;i<parlen;i++)genPoly[i]=exp_table[pG[parlen-i]];

    for(i=0;i<parlen;i++)LFSR[i]=0;

    for(i=0;i<datalen;i++)
    {
        dbyte = databuf[i]^LFSR[parlen-1];
        for(j=parlen-1;j>0;j--)LFSR[j]=LFSR[j-1]^fast_mult(genPoly[j],dbyte);
        LFSR[0]=fast_mult(genPoly[0],dbyte);
    }

    for(i=0;i<parlen;i++)parbuf[i]=LFSR[parlen-1-i];
}

unsigned char QRCode::fast_mult(unsigned char a,unsigned char b)
{
    if((a==0)||(b==0))return 0;
    else return exp_table[log_table[a] + log_table[b]];
}

void QRCode::qrcode_output_data(int mask_type)
{
    unsigned short total_bytes,nblock,nplus,blocksize,parlen;
    unsigned char *ptrData,*ptrPAR;
    int i,j,n;
    plot_mask_type=mask_type;
    total_bytes = QR_TOTAL_SIZE[qr_ver-1];
    nblock = QR_DATA_BLOCK_COUNT[qr_ver-1][qr_rank-1];
    blocksize = QR_DATA_SIZE[qr_ver-1][qr_rank-1]/nblock;
    nplus = nblock - (total_bytes%nblock);
    parlen = total_bytes/nblock-QR_DATA_SIZE[qr_ver-1][qr_rank-1]/nblock;
    plot_data_x=qr_size;
    plot_data_y=qr_size-1;
    plot_data_direct=1;
    ptrData = bitstream_pstart;
    ptrPAR = bitstream_pstart+QR_DATA_SIZE[qr_ver-1][qr_rank-1];
    //output data
    for(i=0;i<blocksize;i++)
    for(j=0;j<nblock;j++)
    {
        if(j<nplus)n=blocksize*j+i;
        else n=blocksize*nplus+(blocksize+1)*(j-nplus)+i;
        plot_data(ptrData[n]);
    }
    for(j=nplus;j<nblock;j++)
    {
        n = blocksize*nplus+(blocksize+1)*(j-nplus)+blocksize;
        plot_data(ptrData[n]);
    }

    //output ecc
    for(i=0;i<parlen;i++)
    for(j=0;j<nblock;j++)
    {
        n=parlen*j+i;
        plot_data(ptrPAR[n]);
    }
    //output blank
    for(i=bitstream_length/8+parlen*nblock;i<total_bytes;i++)
    {
        plot_data(0);
    }
    plot_data(0);
}

void QRCode::qrcode_calc(int ver,int rank,char *buf,int len)
{
	int encode_len=0;
	int i,mode;

	if(rank>4||rank<=0)rank=2;
	qr_rank = rank;
/*
	//按模式进行数据转换
	mode=0;
	//检查是不是全数字
	if(!mode)
	{
		for(i=0;i<len;i++)
		{
			if(buf[i]>='0'&&buf[i]<='9')continue;
			break;
		}
		if(i==len)
		{
			mode=1;
		}
	}
	//检查是不是全字符
	if(!mode)
	{
		for(i=0;i<len;i++)
		{
			if(buf[i]>='0'&&buf[i]<='9')continue;
			if(buf[i]>='A'&&buf[i]<='Z')continue;
			if(buf[i]==' '||buf[i]=='$'||buf[i]=='%'||buf[i]=='*'||buf[i]=='+'||buf[i]=='-'||buf[i]=='.'||buf[i]=='/'||buf[i]==':')continue;
			break;
		}
		if(i==len)mode=2;
	}
	//设置为字节数据模式
//    if(!mode)mode=4;   bug
*/
	 mode=4;
	//估计数据长度
	if(mode==1)encode_len=18+(len/3)*10+((len%3)==2)*7+((len%3)==1)*4;
	else if(mode==2)encode_len=17+(len/2)*11+((len%2)==1)*6;
	else if(mode==4)encode_len=24+len*8;
	encode_len = (encode_len-1)/8+1;
	//判断应该使用的版本
	if(ver==0)
	{
		for(i=0;i<40;i++)if(QR_DATA_SIZE[i][rank-1]>=encode_len){qr_ver=i+1;break;}        
	}
	else
	{
		if(QR_DATA_SIZE[ver-1][rank-1]>=encode_len){qr_ver=ver;}else return;
	}
	qr_size = 21-4+qr_ver*4;
}

int QRCode::qrcode_encode(int ver,int rank,char *buf,int len)
{
	int i;
	int mode=4;
	unsigned char c;

    //根据模式编码原始数据
    bitstream_pstart = (unsigned char *)(buf-3);
    bitstream_length = 0;
    bitstream_add(mode,4);
    if(mode==1)
    {
        unsigned short c;
        if(qr_ver<=9)bitstream_add(len,10);
        else if(qr_ver<=26)bitstream_add(len,12);
        else if(qr_ver<=40)bitstream_add(len,14);
        for(i=0;i<len/3;i++)
        {
            c = (buf[i*3]-'0')*100+(buf[i*3+1]-'0')*10+(buf[i*3+2]-'0');
            bitstream_add(c,10);
        }
        if(len-i*3==2)
        {
            c = (buf[i*3]-'0')*10+(buf[i*3+1]-'0');
            bitstream_add(c,7);
        }
        else if(len-i*3==1)
        {
            c = (buf[i*3]-'0');
            bitstream_add(c,4);
        }
    }
    else if(mode==2)
    {
        unsigned short c;
        if(qr_ver<=9)bitstream_add(len,9);
        else if(qr_ver<=26)bitstream_add(len,11);
        else if(qr_ver<=40)bitstream_add(len,13);
        for(i=0;i<len/2;i++)
        {
            c = ch2num(buf[i*2])*45+ch2num(buf[i*2+1]);
            bitstream_add(c,11);
        }
        if(len-i*2==1)
        {
            c = ch2num(buf[i*2]);
            bitstream_add(c,6);
        }
    }
    //else if(mode==3) bug
    else if(mode==4)
    {
        if(qr_ver<=9)bitstream_add(len,8);
        else if(qr_ver<=26)bitstream_add(len,16);
        else if(qr_ver<=40)bitstream_add(len,16);
        for(i=0;i<len;i++)
        {
            bitstream_add(buf[i],8);
        }
    }
    //填充中止符0
    if(bitstream_length%8)
    {
        bitstream_add(0,8-bitstream_length%8);
    }
    //填充满符号容量
    c = 0xEC;
    for(i=bitstream_length/8;i<QR_DATA_SIZE[qr_ver-1][rank-1];i++)
    {
        bitstream_add(c,8);
        if(c==0xEC)c=0x11;else c=0xEC;
    }
    //计算校验
    {
        unsigned short total_bytes,nblock,blocksize,parlen;
        unsigned char *ptrData,*ptrPAR;
        total_bytes = QR_TOTAL_SIZE[qr_ver-1];
        nblock = QR_DATA_BLOCK_COUNT[qr_ver-1][qr_rank-1];
        blocksize = QR_DATA_SIZE[qr_ver-1][qr_rank-1]/nblock;
        parlen = total_bytes/nblock-QR_DATA_SIZE[qr_ver-1][qr_rank-1]/nblock;
        ptrData = bitstream_pstart;
        ptrPAR = bitstream_pstart+QR_DATA_SIZE[qr_ver-1][qr_rank-1];
        for(i=0;i<nblock;i++)
        {
            if(i==nblock - (total_bytes%nblock))blocksize++;
            reedsolomon_qrcode(ptrData,blocksize,ptrPAR,parlen);
            ptrData+=blocksize;
            ptrPAR+=parlen;
        }
    }

    qr_size = 21-4+qr_ver*4;

    return bitstream_length;
}

unsigned char QRCode::ch2num(unsigned char ch)
{
    if(ch>='0'&&ch<='9')return ch-'0';
    else if(ch>='A'&&ch<='Z')return ch-'A'+10;
    else if(ch==' ')return 36;
    else if(ch=='$')return 37;
    else if(ch=='%')return 38;
    else if(ch=='*')return 39;
    else if(ch=='+')return 40;
    else if(ch=='-')return 41;
    else if(ch=='.')return 42;
    else if(ch=='/')return 43;
    else if(ch==':')return 44;
    return 0;
}

void QRCode::bitstream_add(unsigned short c,int bitlen)
{
    int i,n,b;
    n = bitstream_length/8;
    b = 7-bitstream_length%8;
    for(i=bitlen-1;i>=0;i--)
    {
        if(c&(1<<i)){bitstream_pstart[n]|=(1<<b);}else{bitstream_pstart[n]&=~(1<<b);}
        b--;
        if(b<0){b=7;n++;}
    }
    bitstream_length+=bitlen;
}

void QRCode::plot_data(unsigned char c)
{
    int i,v,b;
    for(i=7;i>=0;i--)
    {
        while(1)
        {
            if(plot_data_x==0&&plot_data_y==qr_size-8)return;
            if(plot_data_x>6)v=(plot_data_x%2==0);
            else v=plot_data_x%2;
            if(v)plot_data_x--;
            else
            {
                if(plot_data_y==(qr_size-1)&&plot_data_direct==1)
                {
                    plot_data_x--;
                    plot_data_direct=-1;
                }
                else if(plot_data_y==0&&plot_data_direct==-1)
                {
                    plot_data_x--;
                    plot_data_direct=1;
                }
                else
                {
                    plot_data_x++;
                    plot_data_y+=plot_data_direct;
                }
            }
            if(plot_data_x==6)plot_data_x=5;
            if(pcolor(plot_data_x,plot_data_y))continue;
            b=((c&(1<<i))!=0);
            if(plot_mask_type==0){if((plot_data_x+plot_data_y)%2==0)b=!b;}
            else if(plot_mask_type==1){if((plot_data_y)%2==0)b=!b;}
            else if(plot_mask_type==2){if((plot_data_x)%3==0)b=!b;}
            else if(plot_mask_type==3){if((plot_data_x+plot_data_y)%3==0)b=!b;}
            else if(plot_mask_type==4){if((plot_data_x/3+plot_data_y/2)%2==0)b=!b;}
            else if(plot_mask_type==5){if((plot_data_x*plot_data_y)%2+(plot_data_x*plot_data_y)%3==0)b=!b;}
            else if(plot_mask_type==6){if(((plot_data_x*plot_data_y)%2+(plot_data_x*plot_data_y)%3)%2==0)b=!b;}
            else if(plot_mask_type==7){if(((plot_data_x*plot_data_y)%3+(plot_data_x+plot_data_y)%2)%2==0)b=!b;}
            if(b)plot(plot_data_x,plot_data_y,1);
            else plot(plot_data_x,plot_data_y,2);
            break;
        }
    }
}

void QRCode::qrcode_make_border(int space_color)
{
    int i,j,k,n,d,sc;
    int x0,y0;
    sc = space_color;
    //画3个位置探测图形
    x0=3,y0=3;
    for(i=0;i<6;i++){plot(x0-3+i,y0-3,1);plot(x0-2+i,y0+3,1);plot(x0-3,y0-2+i,1);plot(x0+3,y0-3+i,1);}
    for(i=0;i<4;i++){plot(x0-2+i,y0-2,sc);plot(x0-1+i,y0+2,sc);plot(x0-2,y0-1+i,sc);plot(x0+2,y0-2+i,sc);}
    for(i=0;i<3;i++){plot(x0-1,y0-1+i,1);plot(x0,y0-1+i,1);plot(x0+1,y0-1+i,1);}
    x0=3,y0=qr_size-4;
    for(i=0;i<6;i++){plot(x0-3+i,y0-3,1);plot(x0-2+i,y0+3,1);plot(x0-3,y0-2+i,1);plot(x0+3,y0-3+i,1);}
    for(i=0;i<4;i++){plot(x0-2+i,y0-2,sc);plot(x0-1+i,y0+2,sc);plot(x0-2,y0-1+i,sc);plot(x0+2,y0-2+i,sc);}
    for(i=0;i<3;i++){plot(x0-1,y0-1+i,1);plot(x0,y0-1+i,1);plot(x0+1,y0-1+i,1);}
    x0=qr_size-4,y0=3;
    for(i=0;i<6;i++){plot(x0-3+i,y0-3,1);plot(x0-2+i,y0+3,1);plot(x0-3,y0-2+i,1);plot(x0+3,y0-3+i,1);}
    for(i=0;i<4;i++){plot(x0-2+i,y0-2,sc);plot(x0-1+i,y0+2,sc);plot(x0-2,y0-1+i,sc);plot(x0+2,y0-2+i,sc);}
    for(i=0;i<3;i++){plot(x0-1,y0-1+i,1);plot(x0,y0-1+i,1);plot(x0+1,y0-1+i,1);}
    //画分隔符
    for(i=0;i<8;i++){plot(i,7,sc);plot(7,i,sc);plot(i,qr_size-8,sc);plot(7,qr_size-8+i,sc);plot(qr_size-8,i,sc);plot(qr_size-8+i,7,sc);}
    //画定位图形(间隔虚线)
    for(i=2;i<qr_size-14;i=i+2){plot(6,6+i,1);plot(6+i,6,1);}
    for(i=3;i<qr_size-14;i=i+2){plot(6,6+i,sc);plot(6+i,6,sc);}
    //画校正图形
    if(qr_size>21)
    {
        n = (qr_size-21+4)/4/7+1;
        d = ((qr_size-13)+(n-1))/n;
        if(d%2)d++;
        for(i=0;i<=n;i++)
            for(j=0;j<=n;j++)
            {
                if(i==n&&j==n)continue;
                if(i==0&&j==n)continue;
                if(i==n&&j==0)continue;
                x0 = qr_size-7-i*d;
                if(x0<6)x0=6;
                y0 = qr_size-7-j*d;
                if(y0<6)y0=6;
                for(k=0;k<4;k++){plot(x0-2+k,y0-2,1);plot(x0-1+k,y0+2,1);plot(x0-2,y0-1+k,1);plot(x0+2,y0-2+k,1);}
                for(k=0;k<2;k++){plot(x0-1+k,y0-1,sc);plot(x0+k,y0+1,sc);plot(x0-1,y0+k,sc);plot(x0+1,y0-1+k,sc);}
                plot(x0,y0,1);
            }
    }
    //画格式信息
    x0 = 8; y0 = 0;
    for(i=0;i<15;i++)
    {
        if(QR_FORMAT&(1<<i))plot(x0,y0,1);else plot(x0,y0,sc);
        if(i==5)y0++;
        if(i==7)y0+=qr_size-16;
        y0++;
    }
    x0 = qr_size-1; y0 = 8;
    for(i=0;i<15;i++)
    {
        if(QR_FORMAT&(1<<i))plot(x0,y0,1);else plot(x0,y0,sc);
        if(i==7)x0-=qr_size-16;
        if(i==8)x0--;
        x0--;
    }
    plot(8,qr_size-8,1);
    if(qr_ver>=7)
    {
        //画版本信息
        x0=0;y0=qr_size-11;
        for(i=0;i<18;i++)
        {
            if(QR_VERSION_INFO[qr_ver-7]&(1<<i)){plot(x0,y0,1);plot(y0,x0,1);}else{plot(x0,y0,sc);plot(y0,x0,sc);}
            y0++;
            if(i%3==2){x0++;y0-=3;}
        }
    }
}

void QRCode::plot(int x,int y,int c)
{
    unsigned char *ptr;
    int n,b;
    ptr = ptrPlot+qr_line_bytes*y;
    n = x/8;
    b = x%8;
    if(c==1)ptr[n]|=(1<<b);
    else ptr[n]&=~(1<<b);
}

int QRCode::pcolor(int x,int y)
{
    unsigned char *ptr;
    int n,b;
    ptr = ptrPlot+qr_line_bytes*y;
    n = x/8;
    b = x%8;
    if(ptr[n]&(1<<b))return 1;
    return 0;
}


