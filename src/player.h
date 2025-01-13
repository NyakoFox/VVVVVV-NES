#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

typedef struct {
    unsigned char x;
    unsigned char y;
    signed char vx;
    signed char vy;
    unsigned char dir;
    unsigned char mood;
    unsigned char frame;
    unsigned char framedelay;
    BOOL flipped;
    BOOL visible;
} player_t;

extern player_t player;

BOOL player_onground(void);

void player_respawn(void);

void player_input(void);
void player_update(void);

void deathsequence(void);
void lifesequence(void);

#endif