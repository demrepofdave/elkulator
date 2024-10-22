/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * video.h
 * 
 * Video abstration layer include file..
 * 
 * Allows actual graphics libraries used for the emulation to be abstracted 
 * from the actual electron code.
 * 
 * This allows easier porting to different graphics and sound libraries in 
 * future in order to allow maximum cross platform support and long term
 * durability.
 *
 * This is the allegro 5 implementation of the abstraction layer.
 *
 */

#ifndef _VIDEO_H
#define _VIDEO_H

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

// Various defines for various type of display and display filters
#define SCANLINES 0
#define LINEDBL   1
#define _2XSAI    2
#define SCALE2X   3
#define EAGLE     4
#define PAL       5

/******************************************************************************
* Typedefs
*******************************************************************************/

// This structure will all information related to the native window state
// and coordinates.

typedef struct
{
    int winsizex;         // Current window size in pixels
    int winsizey;         // Current window size in pixels
} window_info_native_t;
typedef struct
{
    int startx;           // Start position within the native window for the electron window.
    int starty;           // Start position within the native window for the electron window.
    int winsizex;         // Current window size in pixels
    int winsizey;         // Current window size in pixels
} window_info_elk_t;

typedef struct
{
    window_info_native_t actual_window;  // Main window size
    window_info_elk_t current_elk;    // Window size electron
} window_config_t;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

int  video_init_part1();                                // Called from main() in linux.c
void video_init_part2();                                // Called from ulainit() in ula.c
void video_init_part3(void (*timer_function)(void));    // Called from initelk() in main.c

void video_register_close_button_handler(void (*handler_function)(void)); // Only used in allegro4.

int video_set_display_switch_mode_background();
int video_poll_joystick();
void video_rest(unsigned int period);  // Only used in allegro4.

void video_update_native_window_size(int w, int h);
void video_resize_elk_window(bool aspect_ratio);
void video_set_window_size(int w, int h, int v_w, int v_h);
void video_apply_window_size();
void video_set_gfx_mode_windowed();
void video_set_gfx_mode_fullscreen();
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
void video_stop_timer();

#endif // _VIDEO_H
