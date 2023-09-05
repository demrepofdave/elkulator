/*Elkulator v1.0 by Sarah Walker
  Placeholder for global variable definitions (as part of code reorganisation
  Will eventually be moved later */

#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include "elk.h"

int curdrive = 0;
drive_t drives[MAX_DRIVES];
char exedir[512];
int pauseit=0;
int soundon=1;
int output=0;
int timetolive; /* TODO: No initial value? */
int tapelcount,tapellatch;  /* TODO: No initial value? */
int pps; /* TODO: No initial value? */
int cswena;  /* TODO: No initial value? */
int reallyfasttapebreak;   /* TODO: No initial value? */
uint16_t oldpc;   /* TODO: No initial value? */
uint16_t pc;  /* TODO: No initial value? */