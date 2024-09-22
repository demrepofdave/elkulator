#include <allegro.h>
#include "video.h"

// Private functions to be places in hal only header.
BITMAP * getBitmap(bitmapSelect bitmapIndex);

void Super2xSaI_ex(uint8 *src, uint32 src_pitch, uint8 *unused, BITMAP *dest, uint32 width, uint32 height);
void SuperEagle_ex(uint8 *src, uint32 src_pitch, uint8 *unused, BITMAP *dest, uint32 width, uint32 height);