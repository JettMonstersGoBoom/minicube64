uint32_t mfb_getpix(int x,int y);
void mfb_setpix(int x,int y,uint32_t rgba);
void mfb_line( int x0, int y0, int x1, int y1, uint32_t rgba);
void mfb_blit( uint32_t *src, int dx, int dy, int w, int h,int stride);
void mfb_box(int x0,int y0,int x1,int y1,uint32_t rgba);
void mfb_box_fill(int x0,int y0,int x1,int y1,uint32_t rgba);
void mfb_rect( int x, int y, int w, int h, uint32_t rgba);
void mfb_rect_fill( int x, int y, int w, int h, uint32_t rgba);
void mfb_circle(int sx, int sy, int radius, uint32_t rgba);
void mfb_circle_fill(int sx, int sy, int radius, uint32_t rgba);
void mfb_print(int x, int y, uint32_t rgba, const char *text, ...);

#define MFB_RGB(r, g, b)    (((uint32_t) r) << 16) | (((uint32_t) g) << 8) | ((uint32_t) b)
