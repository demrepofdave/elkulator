#include <allegro.h>

const char * fileutils_get_extension(const char * filename)
{
    return get_extension(filename);
}

void fileutils_get_executable_name(char *output, int size)
{
    get_executable_name(output, size);
}

char * fileutils_get_filename(char *path)
{
    return get_filename(path);
}