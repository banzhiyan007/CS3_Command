#include <mbed.h>
#include <graphics.hpp>

void FloydSteinberg(Block *pBlock,int Contrast)
{
	int lbs=((pBlock->Width-1)/8+1);
	int width = pBlock->Width;
	int height = pBlock->Height;

	// -- separate out image components (2)
	int16_t *pixel_floyd = new int16_t[width*height];

	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			if(pBlock->pData[y*lbs+(x/8)]&(1<<(x%8)))
			{
				pixel_floyd[y*width+x]=0xFF-Contrast;
			}
			else
			{
				pixel_floyd[y*width+x]=0xFF;
			}
		}
	}

	int oldpixel, newpixel, error;
	bool nbottom, nleft, nright;

	for (int y = 0; y < height; y++)
	{
		nbottom = y < height - 1;
		for (int x = 0; x < width; x++)
		{
			nleft = x > 0;
			nright = x < width - 1;

			oldpixel = pixel_floyd[y*width+x];
			newpixel = oldpixel < 128 ? 0 : 255;

			pixel_floyd[y*width+x] = newpixel;

			error = oldpixel - newpixel;

			if (nright)
			{
				pixel_floyd[y*width+(x+1)] += 7 * error / 16;
			}
			if (nleft & nbottom)
			{
				pixel_floyd[(y+1)*width+(x-1)] += 3 * error / 16;
			}
			if (nbottom)
			{
				pixel_floyd[(y+1)*width+x] += 5 * error / 16;
			}
			if (nright && nbottom)
			{
				pixel_floyd[(y+1)*width+(x+1)] += error / 16;
			}
		}
	}

	memset(pBlock->pData,0,lbs*pBlock->Height);
	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			if(pixel_floyd[y*width+x]==0)
			{
				pBlock->pData[y*lbs+(x/8)]|=(1<<(x%8));
			}
		}
	}
	delete pixel_floyd;
}