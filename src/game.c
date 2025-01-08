#include "fire.h"
#include "famistudio.h"

#include "helloworld.h"

extern unsigned char music_data_pppppp[];

void nmi_hook() {
    rainbow_color_nmi();
    famistudio_update();
}

void main() {
    draw_hello_world();

    select_data_bank(0x80);
    select_sample_bank(0x83);
    famistudio_init(FAMISTUDIO_PLATFORM_NTSC, &music_data_pppppp);
    famistudio_music_play(0);

    while (1) {
    }
}

#pragma code-name ("PRG_BANK_1")
#pragma rodata-name ("PRG_BANK_1")
#pragma code-name ("DATA_BANK_2")
#pragma rodata-name ("DATA_BANK_2")
