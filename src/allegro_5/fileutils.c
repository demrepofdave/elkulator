#include <allegro5/allegro.h>

char * fileutils_get_extension(char * filename)
{
    // Quick and dirty implementation (allegro 5 does not seem to have the equivalent function).
    char * pExtension = NULL;
    int index = 0;
    if(filename)
    {
        pExtension = filename;
        while(filename[index] != NULL)
        {
            if(filename[index] == '.')
            {
                pExtension = &filename[index];
                pExtension++;
            }
            index++;
        }
    }
    if(pExtension && *pExtension == NULL)
    {
        pExtension = NULL;
    }
    return pExtension;
}

void fileutils_get_executable_name(char *output, int size)
{
    ALLEGRO_PATH * path = al_create_path(".");
    const char * path_string = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);

    if(path_string)
    {
        strncpy(output, path_string, size);
    }
    else
    {
        output[0] = '\0';  // TODO: This should not be used for allegro5
    }
    al_destroy_path(path);
    return;
}

char * fileutils_get_filename(char *path)
{
    return NULL;
}