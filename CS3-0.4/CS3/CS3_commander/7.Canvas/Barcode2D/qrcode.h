#ifndef QRCODE_H
#define QRCODE_H

#include <Block.hpp>

class QRCode: public Block
{
private:
    int NPAR_MAX;
    unsigned char *ptrPlot;
    int qr_line_bytes;
    int qr_ver,qr_rank,qr_size,qr_mask;
    unsigned short QR_FORMAT;
    int plot_data_x,plot_data_y,plot_data_direct;
    int plot_mask_type;
    unsigned short bitstream_length;
    unsigned char *bitstream_pstart;
public:
    QRCode(int qrcodeVer,int qrcodeRank,char *code);
private:
    void reedsolomon_qrcode(unsigned char *databuf,int datalen,unsigned char *parbuf,int parlen);
    unsigned char fast_mult(unsigned char a,unsigned char b);
    void qrcode_output_data(int mask_type);
	void qrcode_calc(int ver,int rank,char *buf,int len);
    int qrcode_encode(int ver,int rank,char *buf,int len);
    unsigned char ch2num(unsigned char ch);
    void bitstream_add(unsigned short c,int bitlen);
    void plot_data(unsigned char c);
    void qrcode_make_border(int space_color);
    void plot(int x,int y,int c);
    int pcolor(int x,int y);
};

#endif

