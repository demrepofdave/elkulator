
#ifdef HAL_ALLEGRO_5
    #include <allegro5/allegro.h>
#else
    #include <allegro.h>
#endif

#include "hal/hal.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

#define printf rpclog

void rpclog(char *format, ...);

#define SAMPLE_TABLE_SIZE 256

//#define printf rpclog

void rpclog(char *format, ...);

extern hal_result isHalInitialised();

typedef struct tsampleTableEntry
{
    bool bAllocated;
    //SAMPLE * pSample;
    void * pNull;  // May not be needed. We shall see (could be due to buffer overrun bug).

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
        if(sample_table[handle].bAllocated == false)
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
            printf("Handle %d : SAMPLE pointer --\n", index);
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
        sample_table[index].bAllocated = false;
        //sample_table[index].pSample = NULL;
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
        bool bFound = false;

        while(!bFound && index < SAMPLE_TABLE_SIZE)
        {
            if(!sample_table[index].bAllocated)
            {
                // Found a free slot. Use this.
                handle = index;
                sample_table[index].bAllocated = true;
                bFound = true;
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
        sample_table[handle].bAllocated = false;
        //sample_table[handle].pSample = NULL;
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
        //sample_table[handle].pSample = load_wav(filename);
        result = HAL_OK;
    }
    hal_debug_print_allocated_sample_table();
    printf("hal_sample_load_wav: SAMPLE returns %d\n", handle);
    return result;
}

hal_sample_handle hal_allocate_sample_and_load_wav(const char *filename)
{
    hal_sample_handle handle = hal_allocate_sample();
    if(handle >= 0) // Valid handle.
    {
        if(hal_sample_load_wav(handle, filename) != HAL_OK)
        {
            hal_release_sample(handle);
            handle = -1; // TODO: Simple error for now.
        }
    }
    return handle;
}

hal_result hal_destroy_sample(hal_bitmap_handle handle)
{
    hal_result result = isSampleAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        //destroy_sample(sample_table[handle].pSample);
    }

    hal_debug_print_allocated_sample_table();
    
    return result;
}


// Sound manipulation functions.

unsigned long  hal_sample_get_length(hal_sample_handle handle)
{
    unsigned long len = -1;
    hal_result result = isSampleAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        len = 0; //sample_table[handle].pSample->len;
    }

    return len;
}

void * hal_sample_get_data_ptr(hal_sample_handle handle)
{
    void * data_ptr = NULL;
    hal_result result = isSampleAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        data_ptr = NULL; // sample_table[handle].pSample->data;
    }

    return data_ptr;
}

int hal_sample_get_frequency(hal_sample_handle handle)
{
    int freq = -1;
    hal_result result = isSampleAssigned(handle);

    if(result == HAL_OK)
    {
        // TODO: Logic stuff
        freq = 0; // sample_table[handle].pSample->freq;
    }

    return freq;
}

// End of file.