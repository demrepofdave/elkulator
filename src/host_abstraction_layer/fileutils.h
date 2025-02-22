/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * fileutils.h
 * 
 * A collection of file utilities for such things as 
 * extracting the extension or executable name from
 * a path + filename combination.
 * 
 */

#ifndef _FILEUTILS_H
#define _FILEUTILS_H

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

const char * fileutils_get_extension(const char * filename);
void         fileutils_get_executable_name(char *output, int size);
char *       fileutils_get_filename(char *path);

#endif // _FILEUTILS_H