#include "graphics.h"

#include "player.h"

#include "fire.h" // hardware headers
#include "rapidfire.h"

extern char nmi_oam_enable;
#pragma zpsym ("nmi_oam_enable");

typedef struct {
    unsigned char y;
    unsigned char tile_index;
    unsigned char attributes;
    unsigned char x;
} sprite_t;

extern sprite_t oam_shadow[];

unsigned char current_sprite;

extern unsigned int* cptr;
#pragma zpsym ("cptr");

unsigned char to_letter(char c)
{
    return c + 0x60;
}

void upload_nametable(const unsigned char* data)
{
    unsigned short i;

    PPU_MASK = MASK_HIDE_BG | MASK_HIDE_SPRITE;
    PPU_CTRL = CTRL_NAMETABLE_2000 | CTRL_INCREMENT_1 | CTRL_SPRITE_0000 | CTRL_BG_0000 | CTRL_SPRITE_8x8 | CTRL_NMI_DISABLE;

    PPU_ADDR = 0x20; PPU_ADDR = 0x00;
    for (i = 0; i < 1024; ++i)
    {
        PPU_DATA = data[i];
    }

    PPU_MASK = MASK_SHOW_BG | MASK_SHOW_SPRITE | MASK_SHOW_LEFT_SPRITE | MASK_SHOW_LEFT_BG;
    PPU_CTRL = CTRL_NAMETABLE_2000 | CTRL_INCREMENT_1 | CTRL_SPRITE_0000 | CTRL_BG_0000 | CTRL_SPRITE_8x8 | CTRL_NMI_ENABLE;
    PPU_SCROLL = 0; PPU_SCROLL = 0;
}

void write_text(const char* text, unsigned char x, unsigned char y)
{
    PPU_MASK = MASK_HIDE_BG | MASK_HIDE_SPRITE;
    PPU_CTRL = CTRL_NAMETABLE_2000 | CTRL_INCREMENT_1 | CTRL_SPRITE_0000 | CTRL_BG_0000 | CTRL_SPRITE_8x8 | CTRL_NMI_DISABLE;

    PPU_ADDR = 0x20; PPU_ADDR = x + y * 32;
    while(*text) {
        PPU_DATA = to_letter(*text);
        ++text;
    }

    PPU_MASK = MASK_SHOW_BG | MASK_SHOW_SPRITE | MASK_SHOW_LEFT_SPRITE | MASK_SHOW_LEFT_BG;
    PPU_CTRL = CTRL_NAMETABLE_2000 | CTRL_INCREMENT_1 | CTRL_SPRITE_0000 | CTRL_BG_0000 | CTRL_SPRITE_8x8 | CTRL_NMI_ENABLE;
    PPU_SCROLL = 0; PPU_SCROLL = 0;
}

void graphics_begin(void)
{
    current_sprite = 0;
    nmi_oam_enable = 0x02;
}

void graphics_end(void)
{
    unsigned char i = current_sprite;
    for (; i < 64; ++i)
    {
        oam_shadow[i].y = 0xFF;
    }

}

void display_sprite(unsigned char tile_index, unsigned char x, unsigned char y, unsigned char attributes)
{
    if (current_sprite >= 64)
    {
        return;
    }

    oam_shadow[current_sprite].y = y;
    oam_shadow[current_sprite].tile_index = tile_index;
    oam_shadow[current_sprite].x = x;
    oam_shadow[current_sprite].attributes = attributes;

    ++current_sprite;
}

void display_player(void)
{
    unsigned char palette = 0;

    if (deathseq > 0)
    {
        player.frame = 0;
        player.mood = 1;
        palette = 1;
    }

    display_crewmate(player.x, player.y, player.frame, player.mood, player.dir, player.flipped, palette);
}

static unsigned char moods[] = {
    0x01, 0x02, // Happy
    0x03, 0x04  // Sad
};

static unsigned char horizontal_flip_offsets[] = {
    0, 8,
    8, 0
};

static unsigned char vertical_flip_offsets[] = {
    //         v-- extra 0 is for padding (since we left shift by 2 to get the index)
    2, 10, 18, 0,
    16, 8, 0,  0
};

void display_crewmate(unsigned char x, unsigned char y, unsigned char frame, unsigned char mood, unsigned char direction, unsigned char flipped, unsigned char palette)
{
    unsigned char use_x = x - 2;
    unsigned char use_y = y - 4;

    unsigned char attributes = palette;
    unsigned char mood_offset;
    unsigned char horiz_offset;
    unsigned char vert_offset;

    if (direction == 1)
    {
        attributes |= 0b01000000;
    }

    if (flipped == 1)
    {
        attributes |= 0b10000000;
    }

    if (frame == 1)
    {
        if (flipped)
        {
            use_y -= 1;
        }
        else
        {
            use_y += 1;
        }
    }

    mood_offset  = (mood << 1);
    horiz_offset = (direction << 1);
    vert_offset  = (flipped << 2);

    display_sprite(moods[mood_offset    ], use_x + horizontal_flip_offsets[horiz_offset    ], use_y + vertical_flip_offsets[vert_offset    ], attributes);
    display_sprite(moods[mood_offset + 1], use_x + horizontal_flip_offsets[horiz_offset + 1], use_y + vertical_flip_offsets[vert_offset    ], attributes);
    display_sprite(0x11 + (frame << 1),    use_x + horizontal_flip_offsets[horiz_offset    ], use_y + vertical_flip_offsets[vert_offset + 1], attributes);
    display_sprite(0x12 + (frame << 1),    use_x + horizontal_flip_offsets[horiz_offset + 1], use_y + vertical_flip_offsets[vert_offset + 1], attributes);
    display_sprite(0x21 + (frame << 1),    use_x + horizontal_flip_offsets[horiz_offset    ], use_y + vertical_flip_offsets[vert_offset + 2], attributes);
    display_sprite(0x22 + (frame << 1),    use_x + horizontal_flip_offsets[horiz_offset + 1], use_y + vertical_flip_offsets[vert_offset + 2], attributes);
}
