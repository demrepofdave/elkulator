

typedef enum 
{
    BIT_SCREEN,
    BIT_MAIN,
    BIT_B16,
    BIT_B162,
    BIT_VIDB,
    BIT_VP1,
    BIT_VP2
} bitmapSelect;


void video_init();
void video_init2();
void video_set_depth_and_elk_palette();
void video_set_desktop_color_depth();

void video_put_pixel(bitmapSelect bitmapIndex, int y, int x, uint8_t color);

void video_blit(bitmapSelect bitmapSourceIndex, bitmapSelect bitmapDestIndex, int source_x, int source_y, int dest_x, int dest_y, int width, int height);

void video_clearall();

void video_shutdown();


// 2xSaI routines.
#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char

int Init_2xSaI(int depth);
void Super2xSaI(bitmapSelect bitmapSourceIndex, bitmapSelect bitmapDestIndex, int s_x, int s_y, int d_x, int d_y, int w, int h);
void SuperEagle(bitmapSelect bitmapSourceIndex, bitmapSelect bitmapDestIndex, int s_x, int s_y, int d_x, int d_y, int w, int h);

// Other filer routines.
void scale2x(bitmapSelect bitmapSourceIndex, bitmapSelect bitmapDestIndex, int width, int height);
void palfilter(bitmapSelect bitmapSourceIndex, bitmapSelect bitmapDestIndex, int depth);

