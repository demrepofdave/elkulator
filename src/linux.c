/*Elkulator v1.0 by Sarah Walker
  Linux main loop*/

#ifdef HAL_ALLEGRO_5
#include <allegro5/allegro.h>
#else
#include <allegro.h>
#endif

#include "elk.h"
#include "common/video.h"

char ssname[260];
char scrshotname[260];
char moviename[260];

int fullscreen=0;
int gotofullscreen=0;
int videoresize=0;
int wantloadstate=0,wantsavestate=0;
int winsizex=640,winsizey=512;

int plus3=0;
int dfsena=0,adfsena=0;
int turbo=0;
int mrb=0,mrbmode=0;
int ulamode=0;
int drawmode=0;

char discname[260];
char discname2[260];
int quited=0;
int infocus=1;

void native_window_close_button_handler(void)
{
       quited = 1;
}

int main(int argc, char *argv[])
{
        int ret = video_init_part1();
        if (ret != 0)
        {
                fprintf(stderr, "Error %d initializing Allegro.\n", ret);
                exit(-1);
        }
        initelk(argc,argv);
        video_register_close_button_handler(native_window_close_button_handler);
        
        while (!quited)
        {
                runelk();
                if (menu_pressed()) entergui();
        }
        closeelk();
        return 0;
}

#ifndef HAL_ALLEGRO_5
END_OF_MAIN();
#endif // HAL_ALLEGRO_5

