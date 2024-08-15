#include <allegro.h>
#include "hal/hal.h"
#include "hal/2xsai.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"


#define BITMAP_TABLE_SIZE 256
#define SAMPLE_TABLE_SIZE 256

static bool hal_initialized = FALSE;

PALETTE elkpal =
{
      {0,0,0},
      {63,0,0},
      {0,63,0},
      {63,63,0},
      {0,0,63},
      {63,0,63},
      {0,63,63},
      {63,63,63},
};

typedef struct tbitmapTableEntry
{
    bool bAllocated;
    BITMAP * pBitmap;

} bitmap_table_entry;

typedef struct tsampleTableEntry
{
    bool bAllocated;
    SAMPLE * pSample;

} sample_table_entry;

static bitmap_table_entry bitmap_table[BITMAP_TABLE_SIZE];
static sample_table_entry sample_table[SAMPLE_TABLE_SIZE];

// Private functions.
hal_result isHalInitialised()
{
    hal_result result = HAL_UNINITIALISED;

    if(hal_initialized)
    {
        result = HAL_OK;
    }
    return result;
}

hal_result isValidHandle(hal_bitmap_handle handle)
{
    // Also check hal is initialized (can't be a valid handle if hal is not initialized)
    hal_result result = isHalInitialised();

    if(result = HAL_OK && (handle < 0 || handle >= BITMAP_TABLE_SIZE))
    {
        result = HAL_INVALID_HANDLE;
    }

    return result;
}

hal_result isValidUserHandle(hal_bitmap_handle handle)
{
    // Also check hal is initialized (can't be a valid handle if hal is not initialized)
    hal_result result = isHalInitialised();

    if(result = HAL_OK && (handle < 1 || handle >= BITMAP_TABLE_SIZE)) // Don't include screen handle (at index 0) - not a user defined handle.
    {
        result = HAL_INVALID_HANDLE;
    }

    return result;
}

hal_result isUserBitmapAssigned(hal_bitmap_handle handle)
{
    // Also check hal is initialized (can't be a valid handle if hal is not initialized)
    hal_result result = isValidUserHandle(handle);

    if(result == HAL_OK)
    {
        if(bitmap_table[handle].bAllocated == FALSE)
        {
            result = HAL_BITMAP_UNALLOCATED;
        }
    }

    return result;
}

// Private debug functions.
hal_result hal_debug_print_allocated_bitmap_table()
{
    // TODO: Init check.
    printf("HAL Bitmap Table - All Allocated entries\n");
    
    for(int index = 0; index < BITMAP_TABLE_SIZE; index++)
    {
        if(bitmap_table[index].bAllocated)
        {
            printf("Handle %d : BITMAP pointer %p\n", index, bitmap_table[index].pBitmap);
        }
    }
    return HAL_OK;
}


// Private initialisation functions.

hal_result hal_init_bitmap()
{
    // Reset the bitmap table.
    // Index 0 always pointe to the screen (in gfx.h)
    hal_result result = HAL_OK;
    int index = 0;
    bitmap_table[0].bAllocated = TRUE;
    bitmap_table[0].pBitmap = screen;
    for(index = 1; index < BITMAP_TABLE_SIZE; index++)
    {
        bitmap_table[index].bAllocated = FALSE;
        bitmap_table[index].pBitmap = NULL;
    }
    hal_debug_print_allocated_bitmap_table();
    return result;
}

hal_result hal_init_samples()
{
    // Reset the sound samples table.
    hal_result result = HAL_OK;
    int index = 0;

    for(index = 0; index < BITMAP_TABLE_SIZE; index++)
    {
        sample_table[index].bAllocated = FALSE;
        sample_table[index].pSample = NULL;
    }
    hal_initialized = TRUE;
    hal_debug_print_allocated_bitmap_table();
    return result;
}

// Public functions.
// Initialise the HAL. This must be done before any other API calls to the HAL are made.
hal_result hal_init()
{
    hal_result result = HAL_OK;

    int ret = allegro_init();
    if (ret != 0)
    {
        result = HAL_UNINITIALISED;
    }
    //if(hal_initialized)
    //{
        // Perform any resetting.
    //}

    if(result == HAL_OK)
    {
        result = hal_init_bitmap();
    }

    if(result == HAL_OK)
    {
        result = hal_init_samples();
    }

    if(result == HAL_OK)
    {
        hal_initialized = TRUE;
    }

    printf("hal_init: Complete - Init = %d, result = %d\n", hal_initialized, HAL_OK);
    return result;
}

hal_result hal_set_window_title(const char * name)
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        set_window_title(name);
    }
    return result;
}

hal_result hal_set_close_button_callback( void (*handler_function)(void))
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        set_close_button_callback(handler_function);
    }
    return result;

}

hal_result hal_install_mouse()
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        install_mouse();
    }
    return result;
}

hal_result hal_set_elk_palette()
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        set_palette(elkpal);
    }
    return result;
}

int hal_get_desktop_color_depth()
{
    int depth = -1;
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        depth = desktop_color_depth();
    }
    return depth;
}

hal_result hal_set_color_depth(int depth)
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        set_color_depth(depth);
    }
    return result;
}

hal_result hal_set_gfx_mode_autodetect_windowed(int w, int h, int v_w, int v_h)
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        set_gfx_mode(GFX_AUTODETECT_WINDOWED, w, h, v_w, v_h);
    }
    return result;
}

hal_result hal_set_gfx_mode_autodetect_fullscreen(int w, int h, int v_w, int v_h)
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, w, h, v_w, v_h);
    }
    return result;
}

hal_result hal_init_2xSaI(int depth)
{
    hal_result result = isHalInitialised();
    if(result == HAL_OK)
    {
        Init_2xSaI(depth);
    }
    return result;
}


hal_bitmap_handle hal_allocate_bitmap() // Returns a handler to a BITMAP type and initialises the base BITMAP
{
    hal_bitmap_handle handle = HAL_UNINITIALISED; // HAL not initialised error
    if(isHalInitialised() == HAL_OK)
    {
        // Speed isn't important here so we plod along.
        handle = HAL_HANDLE_TABLE_FULL;
        int index = 1;
        bool bFound = FALSE;

        while(!bFound && index < BITMAP_TABLE_SIZE)
        {
            if(!bitmap_table[index].bAllocated)
            {
                // Found a free slot. Use this.
                handle = index;
                bitmap_table[index].bAllocated = TRUE;
                bFound = TRUE;
            }
            index++;
        }
    }
    hal_debug_print_allocated_bitmap_table();
    // Nothing more to do, handle will be set to handle index
    // or not found at this point
    printf("hal_allocate_bitmap: Allocated handle %d\n", handle);
    return handle;
}

// Releases bitmap and returns handler to the pool.
hal_result hal_release_bitmap(hal_bitmap_handle handle)
{
    hal_result result = isUserBitmapAssigned(handle);

    if(result == HAL_OK)
    {
        // Do destroy bitmap stuff.
        // Return slot to pool.
        bitmap_table[handle].bAllocated = FALSE;
        bitmap_table[handle].pBitmap = NULL;
    }
    hal_debug_print_allocated_bitmap_table();
    // Nothing more to do, handle will be set to handle index
    // or not found at this point.
    printf("hal_release_bitmap: Deallocated handle %d\n", handle);
    return result;
}


// Create bitmap.
hal_result hal_create_bitmap(hal_bitmap_handle handle, int width, int height)
{
    printf("hal_create_bitmap: handle %d, width %d, height %d\n", handle, width, height);
    hal_result result = isUserBitmapAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Error checking that bitmap isn't already present.
        // TODO: Error handling of function result.
        bitmap_table[handle].pBitmap = create_bitmap(width, height);
        result = HAL_OK;
    }
    hal_debug_print_allocated_bitmap_table();
    printf("hal_create_bitmap: BITMAP returns %d\n", handle);
    return result;
}

hal_result hal_create_bitmap_ex(hal_bitmap_handle handle, int color_depth, int width, int height)
{
    printf("hal_create_bitmap_ex: handle %d, width %d, height %d\n", handle, width, height);
    hal_result result = isUserBitmapAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Error checking that bitmap isn't already present.
        // TODO: Error handling of function result.
        bitmap_table[handle].pBitmap = create_bitmap_ex(color_depth, width, height);
        result = HAL_OK;
    }
    hal_debug_print_allocated_bitmap_table();
    printf("hal_create_bitmap_ex: BITMAP returns %d\n", handle);
    return result;

}

hal_result hal_destroy_bitmap(hal_bitmap_handle handle)
{
    hal_result result = isUserBitmapAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        destroy_bitmap(bitmap_table[handle].pBitmap);
    }

    hal_debug_print_allocated_bitmap_table();
    return result;
}

// Blit to global screen variable.
hal_result hal_blit_screen(hal_bitmap_handle source, int source_x, int source_y, int dest_x, int dest_y, int width, int height)
{
    //printf("blitscr %d\n", source);
    hal_result result = isUserBitmapAssigned(source);

    if(result == HAL_OK)
    {
        // TODO: Error handling of function result.
        blit(bitmap_table[source].pBitmap, screen, source_x, source_y, dest_x, dest_y, width, height); // dest = screen.
    }
    return result;
}

// General blit.
hal_result hal_blit(hal_bitmap_handle source, hal_bitmap_handle dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height)
{
    hal_result result = isUserBitmapAssigned(source); // Check 1st handle.

    if(result == HAL_OK)
    {
        result = isValidUserHandle(dest);    // Check 2nd handle.
    }

    if(result == HAL_OK)
    {
        if(source == 0)
        {
            // Update with latest screen pointer.
            bitmap_table[source].pBitmap = screen;
        }
        
        blit(bitmap_table[source].pBitmap, bitmap_table[dest].pBitmap,  source_x, source_y, dest_x, dest_y, width, height);
    }
    return result;
}

// Clears a particular bitmap (back to black?)
hal_result hal_clear_bitmap(hal_bitmap_handle handle)
{
    printf("hal_clear_bitmap: handle %d\n", handle);
    hal_debug_print_allocated_bitmap_table();
    hal_result result = isUserBitmapAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        if(handle == 0)
        {
            clear(screen); // TODO: Tidy this up.
        }
        else
        {
            clear(bitmap_table[handle].pBitmap);
        }
    }

    printf("hal_clear_bitmap: result %d\n", result);
    return result;
}

void * hal_get_bitmap_dat_ex(hal_bitmap_handle handle)
{
    void * data = NULL;
    hal_result result = isUserBitmapAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        data = bitmap_table[handle].pBitmap->dat;
    }

    return data;
}


hal_result hal_Super2xSaI(hal_bitmap_handle source, hal_bitmap_handle dest, int s_x, int s_y, int d_x, int d_y, int w, int h)
{
    hal_result result = isUserBitmapAssigned(source); // Check 1st handle.

    if(result == HAL_OK)
    {
        result = isUserBitmapAssigned(dest);    // Check 2nd handle.
    }

    if(result == HAL_OK)
    {
        Super2xSaI(bitmap_table[source].pBitmap , bitmap_table[dest].pBitmap, s_x, s_y, d_x, d_y, w, h);
    }
    return result;
}

hal_result hal_SuperEagle(hal_bitmap_handle source, hal_bitmap_handle dest, int s_x, int s_y, int d_x, int d_y, int w, int h)
{
    hal_result result = isUserBitmapAssigned(source); // Check 1st handle.

    if(result == HAL_OK)
    {
        result = isUserBitmapAssigned(dest);    // Check 2nd handle.
    }

    if(result == HAL_OK)
    {
        SuperEagle(bitmap_table[source].pBitmap , bitmap_table[dest].pBitmap, s_x, s_y, d_x, d_y, w, h);
    }

    return result;
}

hal_result hal_scale2x(hal_bitmap_handle source, hal_bitmap_handle dest, int width, int height)
{
    hal_result result = isUserBitmapAssigned(source); // Check 1st handle.

    if(result == HAL_OK)
    {
        result = isUserBitmapAssigned(dest);    // Check 2nd handle.
    }

    if(result == HAL_OK)
    {
        scale2x(bitmap_table[source].pBitmap , bitmap_table[dest].pBitmap, width, height);
    }

    return result;
}

hal_result hal_palfilter(hal_bitmap_handle source, hal_bitmap_handle dest, int depth)
{
    hal_result result = isUserBitmapAssigned(source); // Check 1st handle.

    if(result == HAL_OK)
    {
        result = isUserBitmapAssigned(dest);    // Check 2nd handle.
    }

    if(result == HAL_OK)
    {
        palfilter(bitmap_table[source].pBitmap , bitmap_table[dest].pBitmap, depth);
    }

    return result;
}

hal_result hal_save_bmp(hal_bitmap_handle handle, const char *filename)
{
    hal_result result = isUserBitmapAssigned(handle); // Check 1st handle.

    if(result == HAL_OK)
    {
        save_bmp(filename,bitmap_table[handle].pBitmap, NULL);
    }

    return result;
}


hal_result hal_bitmap_setpixel(hal_bitmap_handle handle, int y, int x, uint8_t col)
{
    hal_result result = isUserBitmapAssigned(handle); // Check 1st handle.

    if(result == HAL_OK)
    {
        bitmap_table[handle].pBitmap->line[y][x] = col;
    }

    return result;
}

// End of file.