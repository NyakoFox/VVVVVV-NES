#include "fire.h"
#include "famistudio.h"
#include "rapidfire.h"

#include "game.h"

extern unsigned char music_data_pppppp[];

void nmi_hook() {
    game_nmi();
}

void main() {
    game_init();

    select_data_bank(0x80);
    select_sample_bank(0x83);
    famistudio_init(FAMISTUDIO_PLATFORM_NTSC, &music_data_pppppp);
    famistudio_music_play(0);

    while (1) {
        while (fire_logic)
        {
            fire_logic = FALSE;
            ++frames;
            game_logic();
            game_render();
        }
    }
}

#pragma code-name ("PRG_BANK_1")
#pragma rodata-name ("PRG_BANK_1")
#pragma code-name ("DATA_BANK_2")
#pragma rodata-name ("DATA_BANK_2")
