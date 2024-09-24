#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h>

#define SCANLINES 0
#define LINEDBL   1
#define _2XSAI    2
#define SCALE2X   3
#define EAGLE     4
#define PAL       5

int  video_init_part1();                                // Called from main() in linux.c
void video_init_part2();                                // Called from ulainit() in ula.c
void video_init_part3();                                // Called from ulainit() in ula.
void video_init_part3a(void (*timer_function)(void));   // Called from initelk() in main.c
void video_init_part4(void (*handler_function)(void));  // Called from main() in linux.c
int video_set_display_switch_mode_background();
int video_poll_joystick();
void video_rest(unsigned int period);

void video_set_gfx_mode_windowed(int w, int h, int v_w, int v_h);
void video_set_gfx_mode_fullscreen(int w, int h, int v_w, int v_h);
void video_set_depth_and_elk_palette();
void video_set_desktop_color_depth();
int  video_get_desktop_color_depth();

void video_put_pixel(int y, int x, uint8_t color); // TODO: Depricated
void video_put_pixel_line(int y, int x, int width, uint8_t color);

void video_blit_to_screen(int drawMode, int winsizeX, int winsizeY, int colDepth);
void video_capture_screenshot(int drawMode, int colDepth);
int  video_save_bmp(const char * filename);
void video_destroy_screenshot();

void video_clearall();

void video_shutdown();

#endif // _VIDEO_H
