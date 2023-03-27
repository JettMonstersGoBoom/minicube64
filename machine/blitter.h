
#define BLITTER_LOC 0x0103
#define BLITTER_TRIGGER 0x010d
#pragma pack(1)
typedef struct 
{
	uint8_t x,y;
	uint8_t w,h;
	uint8_t	stride;
	uint8_t xscale;
	uint8_t yscale;
	uint8_t page;
	uint16_t source;
	uint8_t	shade:4;
	uint8_t	data_mode:2;
	uint8_t	address:1;
	uint8_t	fg_mode:1;
} _blitter_t_;

extern _blitter_t_ *blit;
extern uint8_t color_key;
extern uint32_t bpixels;

#define BLITTER_KEY 0x150	//	NEED to find a better location
void blitData();
