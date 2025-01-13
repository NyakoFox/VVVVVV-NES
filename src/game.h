#ifndef GAME_H
#define GAME_H

#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

extern BOOL fire_logic;
extern unsigned char frames;

extern unsigned char deathseq;
extern unsigned char lifeseq;

extern const unsigned char* room_data;

typedef struct {
    unsigned char player_x;
    unsigned char player_y;
    unsigned char player_dir;
    BOOL player_flipped;

    unsigned char room_x;
    unsigned char room_y;

    unsigned char trinkets[3]; // 24 trinkets (24 bits = 3 bytes)
    unsigned char flags[13];   // 104 flags (104 bits = 13 bytes)
    unsigned char fog[50];     // 400 rooms (400 bits = 50 bytes)
} save_t;

extern save_t save;

void game_init();
void game_logic();
void game_render();
void game_nmi();

#endif