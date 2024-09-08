// For allegro4 we need to directly include allegro to maintain support for END_OF_MAIN()
// This is the best attempt so far to abstract it.

#ifdef HAL_ALLEGRO_5
    #include <allegro5/allegro.h>
#else
    #include <allegro.h>
#endif

// End of file