#include <allegro5/allegro.h>

char * fileutils_get_extension(const char * filename)
{
    return get_extension(filename);
}