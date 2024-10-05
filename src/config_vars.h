#ifndef _CONFIG_VARS_H
#define _CONFIG_VARS_H


#define TAPE_NORMAL       0
#define TAPE_FAST         1
#define TAPE_REALLY_FAST  2

typedef struct 
{
    uint8_t mrb;
    uint8_t mrbmode;
    uint8_t turbo; 
} ExpansionConfig;


typedef struct
{
    uint8_t speed;
} TapeConfig;


typedef struct 
{
    TapeConfig      tape;
    ExpansionConfig expansion;

} Config;

extern Config elkConfig;

#endif // CONFIG_VARS_H