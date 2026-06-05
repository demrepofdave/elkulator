/* Elkulator - Linux joydev joystick reader (Allegro 5 backend)
 *
 * Allegro 5's joystick driver does not enumerate some devices (e.g. the
 * CM5 uConsole pad) that the kernel exposes via /dev/input/jsN.  This small
 * module reads the legacy joydev interface directly, mirroring what the
 * Allegro 4 build did internally, and feeds the ADC / First Byte emulation.
 */
#ifndef _JOYDEV_H
#define _JOYDEV_H

/* Open available joysticks.  Safe to call more than once (no-op after the
 * first successful call).  Called lazily by joydev_poll() if not already done. */
void joydev_init(void);

/* Drain pending events from all open joysticks and update cached state.
 * Cheap (non-blocking reads); call before reading axis/button state. */
void joydev_poll(void);

/* Number of usable joysticks detected (pointer-like devices excluded). */
int  joydev_num_joysticks(void);

/* Axis position scaled to -128..127 (0 if joy/axis out of range). */
int  joydev_get_axis(int joy, int axis);

/* Number of buttons on the given joystick (0 if out of range). */
int  joydev_num_buttons(int joy);

/* Button state: 1 = pressed, 0 = released / out of range. */
int  joydev_get_button(int joy, int btn);

/* Close all joystick file descriptors. */
void joydev_shutdown(void);

#endif /* _JOYDEV_H */
