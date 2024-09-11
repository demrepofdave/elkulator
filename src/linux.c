/*Elkulator v1.0 by Sarah Walker
  Linux main loop*/
#ifndef WIN32

#include <stdlib.h>
#include "elk.h"
#include "hal/hal.h"

char ssname[260];
char scrshotname[260];
char moviename[260];

int fullscreen=0;
int gotofullscreen=0;
int videoresize=0;
int wantloadstate=0,wantsavestate=0;
//int winsizex=640,winsizey=512;

int plus3=0;
int dfsena=0,adfsena=0;
int turbo=0;
int mrb=0,mrbmode=0;
int ulamode=0;
int drawmode=0;

char discname[260];
char discname2[260];
int infocus=1;

void startblit()
{
}
void endblit()
{
}

int keylookup[128];


#endif // WIN32
