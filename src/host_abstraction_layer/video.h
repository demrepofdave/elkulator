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
* Public Function Definitions
*******************************************************************************/

void video_register_close_button_handler(void (*handler_function)(void)); // Only used in allegro4.

int video_set_display_switch_mode_background();
int video_poll_joystick();

void video_enterfullscreen();
void video_leavefullscreen();

void video_set_window_title(char * format, ...);

void video_blit_to_screen(int drawMode, uint8_t * elk_screen_data);
void video_capture_screenshot(int drawMode);
int  video_save_screenshot_bmp(const char * filename);
void video_destroy_screenshot();

void video_clearall();

void video_shutdown();


#endif // _VIDEO_H
