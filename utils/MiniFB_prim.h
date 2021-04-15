uint32_t mfb_getpix(struct mfb_window *window,int x,int y);
void mfb_setpix(struct mfb_window *window,int x,int y,uint32_t rgba);
void mfb_line(struct mfb_window *window, int x0, int y0, int x1, int y1, uint32_t rgba);
void mfb_blit(struct mfb_window *window, uint32_t *src, int dx, int dy, int w, int h,int stride);
void mfb_box(struct mfb_window *window,int x0,int y0,int x1,int y1,uint32_t rgba);
void mfb_box_fill(struct mfb_window *window,int x0,int y0,int x1,int y1,uint32_t rgba);
void mfb_rect(struct mfb_window *window, int x, int y, int w, int h, uint32_t rgba);
void mfb_rect_fill(struct mfb_window *window, int x, int y, int w, int h, uint32_t rgba);
void mfb_circle(struct mfb_window *window,int sx, int sy, int radius, uint32_t rgba);
void mfb_circle_fill(struct mfb_window *window,int sx, int sy, int radius, uint32_t rgba);
void mfb_print(struct mfb_window *window,int x, int y, uint32_t rgba, const char *text, ...);