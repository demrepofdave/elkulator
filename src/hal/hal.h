// Graphic Adaption Layer.

// Provides common API for interaction with Graphics library.
// This allows Elkulator to be compiled with allegro4, allegro5, or a different graphics library
// (Maybe SDL) in future.  The aim is to make it easier to swapout and update the
// native graphics driver portion of the code and separate it from the actual Electron driving
// code

// Following functions have been identified as being needed in allegro4.

#ifndef _HAL_H
#define _HAL_H

#include "stdint.h"

//  Possible API

// Abstract handles

typedef int hal_result; 
typedef int hal_bitmap_handle;
typedef int hal_sample_handle;

// Define hal_result error codes.
#define HAL_OK                  0
#define HAL_UNINITIALISED      -1
#define HAL_INVALID_HANDLE     -2
#define HAL_HANDLE_TABLE_FULL  -3
#define HAL_BITMAP_UNALLOCATED -4

// General hal functions.
hal_result hal_init(); // Initialise the HAL. This must be done before any other API calls to the HAL are made.
hal_result hal_set_window_title(const char * name);
hal_result hal_set_close_button_callback( void (*handler_function)(void));
hal_result hal_install_mouse();
hal_result hal_init_2xSaI(int depth);

// Palette and colour handling functions.
hal_result hal_set_elk_palette();
int        hal_get_desktop_color_depth();
hal_result hal_set_color_depth(int depth);
hal_result hal_set_gfx_mode_autodetect_windowed(int w, int h, int v_w, int v_h);
hal_result hal_set_gfx_mode_autodetect_fullscreen(int w, int h, int v_w, int v_h);

// Bitmap management
hal_bitmap_handle hal_allocate_bitmap(); // Returns a handler to a BITMAP type and initialises the base BITMAP
hal_result hal_create_bitmap(hal_bitmap_handle handle, int width, int height);
hal_result hal_create_bitmap_ex(hal_bitmap_handle handle, int color_depth, int width, int height);
hal_result hal_destroy_bitmap(hal_bitmap_handle handle);
hal_result hal_release_bitmap(hal_bitmap_handle handle); // Releases bitmap and returns handler to the pool.

// Blitting functions
hal_result hal_blit_screen(hal_bitmap_handle source, int source_x, int source_y, int dest_x, int dest_y, int width, int height); 
hal_result hal_blit(hal_bitmap_handle source, hal_bitmap_handle dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);

// Bitmap manipulation functions
hal_result hal_clear_bitmap(hal_bitmap_handle handle);

hal_result hal_Super2xSaI(hal_bitmap_handle source, hal_bitmap_handle dest, int s_x, int s_y, int d_x, int d_y, int w, int h);
hal_result hal_SuperEagle(hal_bitmap_handle source, hal_bitmap_handle dest, int s_x, int s_y, int d_x, int d_y, int w, int h);
hal_result hal_scale2x   (hal_bitmap_handle source, hal_bitmap_handle dest, int width, int height);
hal_result hal_palfilter (hal_bitmap_handle source, hal_bitmap_handle dest, int depth);

hal_result hal_save_bmp(hal_bitmap_handle handle, const char *filename);

hal_result hal_bitmap_setpixel(hal_bitmap_handle handle, int y, int x, uint8_t col);
//b->line[ula.y][(ula.x+x)>>1]=pal[col];

// Sound functions - sample management
hal_sample_handle hal_allocate_sample(); // Returns a handler to a SAMPLE type and initialises the base SAMPLE
hal_sample_handle hal_allocate_sample_and_load_wav(const char *filename); // Returns a handler to a SAMPLE type and initialises the base SAMPLE, then loads the WAV file.
hal_result hal_sample_load_wav(hal_bitmap_handle handle, const char *filename);
hal_result hal_destroy_sample(hal_sample_handle handle);
hal_result hal_release_sample(hal_sample_handle handle); // Releases sample and returns handler to the pool.

// Sound manipulation functions.
unsigned long hal_sample_get_length(hal_sample_handle handle);
void * hal_sample_get_data_ptr(hal_sample_handle handle);
int hal_sample_get_frequency(hal_sample_handle handle);

// Joystick control.
hal_result hal_set_joffset(int offset);
int hal_get_joffset();
uint8_t hal_read_firstbyte();

// ADC including joystick control
hal_result hal_write_adc(uint8_t val);
uint8_t hal_read_adc();
uint8_t hal_get_plus1_stat();

// Misc hal.
void hal_get_executable_name(char *output, int size);
char * hal_get_extension(char *filename);
char * hal_get_filename(char *path);
int hal_install_keyboard();
void hal_install_timer(void);
void hal_install_int_ex_in_ms(void (*handler_function)(void), long speed);
int hal_install_joystick_autodetect();
int hal_set_display_switch_mode_background();
int hal_poll_joystick();
void hal_rest(unsigned int period);

// Functions that break the HAL
void *hal_get_bitmap_dat_ex(hal_bitmap_handle handle);

#endif // _HAL_H

/* DemRepOfDave Notes.

Things that need to be abstracted:

BITMAP * (allegro5 = ALLEGRO_BITMAP)
PALLETE * (allegro5 = ???)

*/