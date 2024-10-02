#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h>
#include <stdbool.h>

#define SCANLINES 0
#define LINEDBL   1
#define _2XSAI    2
#define SCALE2X   3
#define EAGLE     4
#define PAL       5

// Following allegro5 only.
#define ELK_EVENT_EXIT  0x01
#define ELK_EVENT_RESET 0x02
#define ELK_EVENT_TIMER_TRIGGERED 0x04

typedef struct
{
    int winsizex; // Current window size in ???
    int winsizey; // Current window size in ???
    bool maintain_aspect; // If true maintain 4:3 aspect ration (used recalce winsizey based on winsizex)
} WindowCoords;

int  video_init_part1();                                // Called from main() in linux.c
void video_init_part2();                                // Called from ulainit() in ula.c
void video_init_part3(void (*timer_function)(void));    // Called from initelk() in main.c

void video_register_close_button_handler(void (*handler_function)(void)); // Only used in allegro4.

int video_set_display_switch_mode_background();
int video_poll_joystick();
void video_rest(unsigned int period);  // Only used in allegro4.

void video_set_window_size(int w, int h, int v_w, int v_h);
void video_set_gfx_mode_windowed();
void video_set_gfx_mode_fullscreen(int w, int h, int v_w, int v_h);
void video_set_depth_and_elk_palette();
void video_set_desktop_color_depth();
int  video_get_desktop_color_depth();

void video_put_pixel(int y, int x, uint8_t color); // TODO: Depricated
void video_put_pixel_line(int y, int x, int width, uint8_t color);

void video_blit_to_screen(int drawMode, int colDepth);
void video_capture_screenshot(int drawMode, int colDepth);
int  video_save_bmp(const char * filename);
void video_destroy_screenshot();

void video_clearall();

void video_shutdown();

void video_start_timer();
uint32_t video_await_event();

#endif // _VIDEO_H
