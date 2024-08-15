#include <allegro.h>
#include "hal/hal.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

#define SAMPLE_TABLE_SIZE 256


typedef struct tsampleTableEntry
{
    bool bAllocated;
    SAMPLE * pSample;

} sample_table_entry;

static sample_table_entry sample_table[SAMPLE_TABLE_SIZE];

// Private functions.

hal_result isValidSampleHandle(hal_sample_handle handle)
{
    // Also check hal is initialized (can't be a valid handle if hal is not initialized)
    hal_result result = isHalInitialised();

    if(result = HAL_OK && (handle < 0 || handle >= SAMPLE_TABLE_SIZE))
    {
        result = HAL_INVALID_HANDLE;
    }

    return result;
}

hal_result isSampleAssigned(hal_sample_handle handle)
{
    // Also check hal is initialized (can't be a valid handle if hal is not initialized)
    hal_result result = isValidSampleHandle(handle);

    if(result == HAL_OK)
    {
        if(sample_table[handle].bAllocated == FALSE)
        {
            result = HAL_BITMAP_UNALLOCATED;
        }
    }

    return result;
}

// Private debug functions.
hal_result hal_debug_print_allocated_sample_table()
{
    // TODO: Init check.
    printf("HAL Sample Table - All Allocated entries\n");
    
    for(int index = 0; index < SAMPLE_TABLE_SIZE; index++)
    {
        if(sample_table[index].bAllocated)
        {
            printf("Handle %d : SAMPLE pointer %p\n", index, sample_table[index].pSample);
        }
    }
    return HAL_OK;
}


// Private initialisation functions.

hal_result hal_init_sample()
{
    // Reset the sound samples table.
    hal_result result = HAL_OK;
    int index = 0;

    for(index = 0; index < SAMPLE_TABLE_SIZE; index++)
    {
        sample_table[index].bAllocated = FALSE;
        sample_table[index].pSample = NULL;
    }
    hal_debug_print_allocated_sample_table();
    return result;
}

// Public functions.

hal_sample_handle hal_allocate_sample()
{
    hal_sample_handle handle = HAL_UNINITIALISED; // HAL not initialised error
    if(isHalInitialised() == HAL_OK)
    {
        // Speed isn't important here so we plod along.
        handle = HAL_HANDLE_TABLE_FULL;
        int index = 0;
        bool bFound = FALSE;

        while(!bFound && index < SAMPLE_TABLE_SIZE)
        {
            if(!sample_table[index].bAllocated)
            {
                // Found a free slot. Use this.
                handle = index;
                sample_table[index].bAllocated = TRUE;
                bFound = TRUE;
            }
            index++;
        }
    }
    hal_debug_print_allocated_sample_table();
    // Nothing more to do, handle will be set to handle index
    // or not found at this point
    printf("hal_allocate_sample: Allocated handle %d\n", handle);
    return handle;
}

// Releases bitmap and returns handler to the pool.
hal_result hal_release_sample(hal_sample_handle handle)
{
    hal_result result = isSampleAssigned(handle);

    if(result == HAL_OK)
    {
        // Do destroy bitmap stuff.
        // Return slot to pool.
        sample_table[handle].bAllocated = FALSE;
        sample_table[handle].pSample = NULL;
    }
    hal_debug_print_allocated_sample_table();
    // Nothing more to do, handle will be set to handle index
    // or not found at this point.
    printf("hal_sample_bitmap: Deallocated handle %d\n", handle);
    return result;
}


// Create bitmap.
hal_result hal_sample_load_wav(hal_bitmap_handle handle, const char *filename)
{
    printf("hal_sample_load_wav: handle %d, filename '%s'", handle, filename);
    hal_result result = isSampleAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Error checking that bitmap isn't already present.
        // TODO: Error handling of function result.
        sample_table[handle].pSample = load_wav(filename);
        result = HAL_OK;
    }
    hal_debug_print_allocated_sample_table();
    printf("hal_sample_load_wav: SAMPLE returns %d\n", handle);
    return result;
}

hal_result hal_destroy_sample(hal_bitmap_handle handle)
{
    hal_result result = isSampleAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        destroy_sample(sample_table[handle].pSample);
    }

    hal_debug_print_allocated_sample_table();
    
    return result;
}

// End of file.