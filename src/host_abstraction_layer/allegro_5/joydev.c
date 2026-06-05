/* Elkulator - Linux joydev joystick reader (Allegro 5 backend)
 *
 * See joydev.h for rationale.  We read /dev/input/jsN via the kernel joydev
 * interface (struct js_event), the same path the Allegro 4 build relies on.
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include "logger.h"
#include "joydev.h"

#define JOYDEV_MAX_STICKS   4
#define JOYDEV_MAX_AXES     8
#define JOYDEV_MAX_BUTTONS  32
#define JOYDEV_SCAN_NODES   32   /* probe /dev/input/js0 .. js31 */

typedef struct
{
    int      fd;
    char     name[128];
    uint8_t  num_axes;
    uint8_t  num_buttons;
    int      axis[JOYDEV_MAX_AXES];       /* scaled to -128..127  */
    uint8_t  button[JOYDEV_MAX_BUTTONS];  /* 0 = up, 1 = pressed  */
} joydev_stick_t;

static joydev_stick_t sticks[JOYDEV_MAX_STICKS];
static int  num_sticks  = 0;
static bool initialised = false;

/* keyd and similar tools expose pointer devices that also create a jsN node.
 * Those are not real joysticks, so skip anything that names itself as one. */
static bool looks_like_pointer(const char *name)
{
    return strcasestr(name, "mouse") != NULL ||
           strcasestr(name, "pointer") != NULL ||
           strcasestr(name, "trackpad") != NULL;
}

static void open_one(const char *path)
{
    if (num_sticks >= JOYDEV_MAX_STICKS)
        return;

    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        return;

    char name[128] = "Unknown";
    char axes = 0, btns = 0;
    ioctl(fd, JSIOCGNAME(sizeof(name)), name);
    ioctl(fd, JSIOCGAXES, &axes);
    ioctl(fd, JSIOCGBUTTONS, &btns);

    if (axes == 0 || looks_like_pointer(name))
    {
        log_info("joydev: skipping %s ('%s', axes=%d)\n", path, name, axes);
        close(fd);
        return;
    }

    joydev_stick_t *s = &sticks[num_sticks];
    memset(s, 0, sizeof(*s));
    s->fd = fd;
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->num_axes    = (axes > JOYDEV_MAX_AXES)    ? JOYDEV_MAX_AXES    : (uint8_t)axes;
    s->num_buttons = (btns > JOYDEV_MAX_BUTTONS) ? JOYDEV_MAX_BUTTONS : (uint8_t)btns;

    log_info("joydev: stick %d = '%s' (%s) axes=%d buttons=%d\n",
             num_sticks, s->name, path, s->num_axes, s->num_buttons);
    num_sticks++;
}

void joydev_init(void)
{
    if (initialised)
        return;
    initialised = true;
    num_sticks  = 0;

    char path[32];
    for (int i = 0; i < JOYDEV_SCAN_NODES && num_sticks < JOYDEV_MAX_STICKS; i++)
    {
        snprintf(path, sizeof(path), "/dev/input/js%d", i);
        if (access(path, R_OK) == 0)
            open_one(path);
    }

    if (num_sticks == 0)
        log_info("joydev: no joysticks found\n");
}

void joydev_poll(void)
{
    if (!initialised)
        joydev_init();

    struct js_event e;
    for (int i = 0; i < num_sticks; i++)
    {
        joydev_stick_t *s = &sticks[i];
        while (read(s->fd, &e, sizeof(e)) == (ssize_t)sizeof(e))
        {
            uint8_t type = e.type & ~JS_EVENT_INIT;
            if (type == JS_EVENT_AXIS && e.number < JOYDEV_MAX_AXES)
            {
                /* joydev axis range is -32767..32767; scale to -128..127 */
                int v = e.value / 256;
                if (v >  127) v =  127;
                if (v < -128) v = -128;
                s->axis[e.number] = v;
            }
            else if (type == JS_EVENT_BUTTON && e.number < JOYDEV_MAX_BUTTONS)
            {
                s->button[e.number] = e.value ? 1 : 0;
            }
        }
    }
}

int joydev_num_joysticks(void)
{
    return num_sticks;
}

int joydev_get_axis(int joy, int axis)
{
    if (joy < 0 || joy >= num_sticks)        return 0;
    if (axis < 0 || axis >= JOYDEV_MAX_AXES) return 0;
    return sticks[joy].axis[axis];
}

int joydev_num_buttons(int joy)
{
    if (joy < 0 || joy >= num_sticks) return 0;
    return sticks[joy].num_buttons;
}

int joydev_get_button(int joy, int btn)
{
    if (joy < 0 || joy >= num_sticks)          return 0;
    if (btn < 0 || btn >= JOYDEV_MAX_BUTTONS)  return 0;
    return sticks[joy].button[btn];
}

void joydev_shutdown(void)
{
    for (int i = 0; i < num_sticks; i++)
        if (sticks[i].fd >= 0)
            close(sticks[i].fd);
    num_sticks  = 0;
    initialised = false;
}
