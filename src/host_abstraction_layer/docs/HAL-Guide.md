HOST ABSTRACTION LAYER DOCUMENTATION
====================================

TBD - Document being used to make sure API is consistent.

Public API definition
=====================

Initialisation and shutdown
---------------------------

int  hal_init_begin();
void hal_init_complete();
void hal_shutdown();

Main event handler
------------------

uint32_t event_await();



Video handling
--------------

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


Sound handling
--------------

void sound_add(uint8_t dat);
void sound_mixbuffer(int16_t *d);
void sound_givealbufferdd(int16_t *buf);

Sound Sample handling
---------------------

bool sample_seek_load(uint8_t indexA, uint8_t indexB, const char * filename); // Don't know what a and b are at the moment.
unsigned long sample_seek_get_length(uint8_t indexA, uint8_t indexB);
void * sample_seek_get_data_ptr(uint8_t indexA, uint8_t indexB);
int  sample_seek_get_frequency(uint8_t indexA, uint8_t indexB);
void sample_seek_destroy_all();

bool sample_motor_load(uint8_t index, const char * filename); // Don't know what index is at the moment.
unsigned long sample_motor_get_length(uint8_t index);
void * sample_motor_get_data_ptr(uint8_t index);
int  sample_motor_get_frequency(uint8_t index);
void sample_motor_destroy_all();

bool sample_tape_noise_motor_load(uint8_t index, const char * filename); // Don't know what index is at the moment.
unsigned long sample_tape_noise_get_length(uint8_t index);
void * sample_tape_noise_get_data_ptr(uint8_t index);
int  sample_tape_noise_get_frequency(uint8_t index);
void sample_tape_noise_destroy_all();


Timer functions
---------------

void hal_install_timer_callback(void (*timer_function)(void));
void hal_start_timer();
void hal_stop_timer();
void hal_timer_rest(unsigned int period);

File utilities
--------------

const char * fileutils_get_extension(const char * filename);
void         fileutils_get_executable_name(char *output, int size);
char *       fileutils_get_filename(char *path);

Keyboard utilties
-----------------

host_key_t keyutils_get_hostkey_from_legacy_keyid(uint8_t old_key_id);
void       keyutils_refresh_elkkeys();  // Allegro 4 only.