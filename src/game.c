#include "game.h"

#include "player.h"

#include "block.h"
#include "fire.h" // hardware headers
#include "easy_joy.h"
#include "graphics.h"
#include "rapidfire.h"

unsigned char frames = 0;
BOOL fire_logic = FALSE;

unsigned char deathseq = 0;
unsigned char lifeseq = 0;

const unsigned char* room_data;

extern unsigned char testingnam[];

save_t save;

void ppu_setup()
{
    PPU_MASK = MASK_HIDE_BG | MASK_HIDE_SPRITE;
    PPU_CTRL = CTRL_NAMETABLE_2000 | CTRL_INCREMENT_1 | CTRL_SPRITE_1000 | CTRL_BG_0000 | CTRL_SPRITE_8x8 | CTRL_NMI_DISABLE;

    PPU_ADDR = 0x3F; PPU_ADDR = 0x00; // universal bg color
    PPU_DATA = 0x0F; // black

    // SETUP PALETTE DATA

    // SPRITES - Palette 0
    PPU_ADDR = 0x3F; PPU_ADDR = 0x11; PPU_DATA = 0x3C; // Viridian Cyan
    PPU_ADDR = 0x3F; PPU_ADDR = 0x12; PPU_DATA = 0x00; // Inactive checkpoint/terminal Gray
    PPU_ADDR = 0x3F; PPU_ADDR = 0x13; PPU_DATA = 0x24; // Trinket purple

    // SPRITES - Palette 1
    PPU_ADDR = 0x3F; PPU_ADDR = 0x15; PPU_DATA = 0x06; // Dead Viridian Red
    PPU_ADDR = 0x3F; PPU_ADDR = 0x16; PPU_DATA = 0x30; // Active checkpoint/terminal White
    PPU_ADDR = 0x3F; PPU_ADDR = 0x17; PPU_DATA = 0x37; // Coin Yellow (temporary)

    // BG - Palette 1
    PPU_ADDR = 0x3F; PPU_ADDR = 0x05; PPU_DATA = 0x30; // White
    PPU_ADDR = 0x3F; PPU_ADDR = 0x06; PPU_DATA = 0x30; // White
    PPU_ADDR = 0x3F; PPU_ADDR = 0x07; PPU_DATA = 0x30; // White

    // BG - Palette 3
    PPU_ADDR = 0x3F; PPU_ADDR = 0x0D; PPU_DATA = 0x0C; // Darker Cyan
    PPU_ADDR = 0x3F; PPU_ADDR = 0x0E; PPU_DATA = 0x1C; // Dark Cyan
    PPU_ADDR = 0x3F; PPU_ADDR = 0x0F; PPU_DATA = 0x2C; // Cyan

    wait_for_vblank();

    // enable rendering
    PPU_MASK = MASK_SHOW_BG | MASK_SHOW_SPRITE | MASK_SHOW_LEFT_SPRITE | MASK_SHOW_LEFT_BG;
    PPU_CTRL = CTRL_NAMETABLE_2000 | CTRL_INCREMENT_1 | CTRL_SPRITE_1000 | CTRL_BG_0000 | CTRL_SPRITE_8x8 | CTRL_NMI_ENABLE;
    PPU_SCROLL = 0; PPU_SCROLL = 0;
}

#define MULT_X (i & 0b00011111) << 3
#define MULT_Y ((i >> 5) << 3)



void load_room(const unsigned char* data)
{
    upload_nametable(data);
    room_data = data;
}

void game_init()
{
    // Handles initialization of the game
    ppu_setup();

    //write_text("Viridian this isn't the ship", 2, 0);
    //write_text("shesay do you love me i tell her !@#@!*#(T$%%)^#R(*&#%^&}|>?)", 8, 2);
    load_room(testingnam);

    save.player_x = 127;
    save.player_y = 120;
    save.player_flipped = FALSE;

    player_respawn();
}

void game_logic()
{
    if (deathseq > 0)
    {
        deathsequence();
    }
    else
    {
        lifesequence();
        player_input();
        player_update();
    }
}

void game_render()
{
    graphics_begin();

    if (player.visible)
    {
        display_player();
    }

    graphics_end();
}

void game_nmi() {
    fire_logic = TRUE;
}
