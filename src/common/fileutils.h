#ifndef _FILEUTILS_H
#define _FILEUTILS_H

char * fileutils_get_extension(const char * filename);
void fileutils_get_executable_name(char *output, int size);
char * fileutils_get_filename(char *path);

#endif // _FILEUTILS_H