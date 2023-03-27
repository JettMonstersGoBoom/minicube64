
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "fake6502.h"
#include "machine.h"
#include "blitter.h"
uint32_t goal_cycles;

#define VWIDTH 64
#define VHEIGHT 64
#define BFLAG_FG_MODE 1<<7 
#define BFLAG_SOLID_MODE 1<<6 

_blitter_t_ *blit=NULL;
bool writePixel = true; 
uint8_t color_key=0;

uint8_t getPixel1(uint16_t source,uint8_t x)
{
	uint8_t b;
	uint8_t bit = 1<<((x)&7);
	b=read6502(source+(x>>3))&bit;
	if (b!=0)
	{
		b = blit->shade;
	}
	else
	{
		if (blit->fg_mode==1)
			writePixel=false;
	}
	return b;
}

uint8_t getPixel2(uint16_t source,uint8_t x)
{
	uint8_t b;
	b=read6502(source+(x>>2));
	uint8_t shf = ((x)&3)<<1;
	b>>=shf;
	b&=3;
	if (((blit->fg_mode)!=0) && (b==0)) writePixel=false;
	b|=blit->shade<<2;
	return b;
}

uint8_t getPixel4(uint16_t source,uint8_t x)
{
	uint8_t b;
	b=read6502(source+(x>>1));
	if (((x)&1)!=0)				
		b>>=4;
	b&=0xf;
	if (((blit->fg_mode)!=0) && (b==0)) writePixel=false;
	b|=blit->shade<<4;
	return b;
}

uint8_t getPixel8(uint16_t source,uint8_t x)
{
	uint8_t b;
	b=read6502(source+x);
	if (((blit->fg_mode)!=0) && (b==color_key)) writePixel=false;
	return b;
}

#define SCALESHIFT 6

uint32_t bpixels = 0;
#define MAX_PIXELS (4096*3)
void blitData()
{
	uint8_t b=rand();;
	uint16_t s = blit->source;
	uint16_t ss = s;
	uint16_t dest = (blit->y<<8) | blit->x;
	uint8_t (*getPixel)(uint16_t source,uint8_t x);
	uint8_t stride=0;
	uint16_t xp;

	if (blit->address)
	{
		blit->page = dest/4096;
		blit->x = dest & 0x3f;
		blit->y = (dest>>6) & 0x3f;
//		printf("%x %d %d %d %d\n",blit->page,blit->x,blit->y,blit->w,blit->h);
	}
	uint8_t *dram = &memory[(blit->page&0xf)*4096];

	switch(blit->data_mode)
	{
		case 0:
		{
			stride = blit->stride>>3;
			getPixel = getPixel1;
			break;
		}
		case 1:
		{
			stride = blit->stride>>2;
			getPixel = getPixel2;
			break;
		}
		case 2:
		{
			stride = blit->stride>>1;
			getPixel = getPixel4;
			break;
		}
		case 3:
		{
			stride = blit->stride;
			getPixel = getPixel8; 
			break;
		}
	}
	int yp=0;
	int y=0;
	while((yp>>SCALESHIFT)<blit->h)
	{
		if (bpixels>=MAX_PIXELS) break;
		if (y>=64) break;
		int x=0;
		xp = 0;
		s = ss+((yp>>SCALESHIFT)*stride);
		while((xp>>SCALESHIFT)<blit->w)
		{
			if (bpixels>=MAX_PIXELS) break;
			if (x>=64) break;
			writePixel=true;
			uint8_t ox = (x+blit->x);
			uint8_t oy = (y+blit->y);
			if ((ox<64) && (oy<64))
			{
				//	can change writePixel to false
				b = getPixel(s,xp>>SCALESHIFT);
				if (writePixel==true)
				{
					dram[ox+(oy*VWIDTH)]=b;
					bpixels++;
					goal_cycles-=1;
					clockticks6502+=1;
				}
				else 
				{
					if (x&1)
					{
						goal_cycles-=1;
						clockticks6502+=1;
					}
				}
			}
			xp+=blit->xscale;
			x++;
		}
		yp+=blit->yscale;
		y++;
	}
}
