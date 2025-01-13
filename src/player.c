#include "player.h"

#include "block.h"
#include "game.h"

#include "fire.h" // hardware headers
#include "rapidfire.h"
#include "easy_joy.h"

player_t player;

static const BOOL solidity_map[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 4, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// Return true if LSB is set -- aka if it's solid
#define CHECK(x,y) if (x & 0b11100000) return FALSE; else if (solidity_map[room_data[x+y]] & 0x01) return TRUE;

BOOL __fastcall__ is_solid_to_player(unsigned char x, unsigned char y)
{
    unsigned char tile_x_left = x >> 3;
    unsigned char tile_x_mid = (x + 5) >> 3;
    unsigned char tile_x_right = (x + 11) >> 3;

    unsigned short tile_y_top = (y << 2) & 0b0000001111100000;
    unsigned short tile_y_mid1 = ((y + 6) << 2) & 0b0000001111100000;
    unsigned short tile_y_mid2 = ((y + 12) << 2) & 0b0000001111100000;
    unsigned short tile_y_bottom = ((y + 19) << 2) & 0b0000001111100000;

    CHECK(tile_x_left, tile_y_top);
    CHECK(tile_x_left, tile_y_mid1);
    CHECK(tile_x_left, tile_y_mid2);
    CHECK(tile_x_left, tile_y_bottom);

    CHECK(tile_x_mid, tile_y_top);
    CHECK(tile_x_mid, tile_y_mid1);
    CHECK(tile_x_mid, tile_y_mid2);
    CHECK(tile_x_mid, tile_y_bottom);

    CHECK(tile_x_right, tile_y_top);
    CHECK(tile_x_right, tile_y_mid1);
    CHECK(tile_x_right, tile_y_mid2);
    CHECK(tile_x_right, tile_y_bottom);

    return FALSE;
}

#undef CHECK

// Kill player if they touch a hazard (right now this is just a check for tiles that aren't air and aren't solid)
#define CHECK(idx) if (solidity_map[room_data[idx]] >> 1) { deathseq = 60; return; };

static inline void check_hazards(void)
{
    unsigned char tile_x_left = player.x >> 3;
    unsigned char tile_x_mid = (player.x + 5) >> 3;
    unsigned char tile_x_right = (player.x + 11) >> 3;

    unsigned short tile_y_top = (player.y << 2) & 0b0000001111100000;
    unsigned short tile_y_mid1 = ((player.y + 6) << 2) & 0b0000001111100000;
    unsigned short tile_y_mid2 = ((player.y + 12) << 2) & 0b0000001111100000;
    unsigned short tile_y_bottom = ((player.y + 19) << 2) & 0b0000001111100000;

    CHECK(tile_x_left + tile_y_top);
    CHECK(tile_x_left + tile_y_mid1);
    CHECK(tile_x_left + tile_y_mid2);
    CHECK(tile_x_left + tile_y_bottom);

    CHECK(tile_x_mid + tile_y_top);
    CHECK(tile_x_mid + tile_y_mid1);
    CHECK(tile_x_mid + tile_y_mid2);
    CHECK(tile_x_mid + tile_y_bottom);

    CHECK(tile_x_right + tile_y_top);
    CHECK(tile_x_right + tile_y_mid1);
    CHECK(tile_x_right + tile_y_mid2);
    CHECK(tile_x_right + tile_y_bottom);
}

#undef CHECK

void player_respawn(void)
{
    player.x = save.player_x;
    player.y = save.player_y;
    player.vx = 0;
    player.vy = 0;
    player.dir = save.player_dir;
    player.mood = 0;
    player.frame = 0;
    player.flipped = save.player_flipped;
    player.framedelay = 0;
    player.visible = TRUE;

    deathseq = 0;
    lifeseq = 20;
}

static signed char flip_velocity[] = { 8, -8 };

void player_input(void)
{
    if (JOY1_HELD & BUTTON_LEFT)
    {
        player.vx -= 1;
        player.dir = 1;
    }
    else
    {
        if (player.vx < 0)
        {
            player.vx += 1;
        }
    }

    if (JOY1_HELD & BUTTON_RIGHT)
    {
        player.vx += 1;
        player.dir = 0;
    }
    else
    {
        if (player.vx > 0)
        {
            player.vx -= 1;
        }
    }

    if (player_onground())
    {
        if (JOY1_PRESSED & BUTTON_A)
        {
            player.flipped ^= 1;
            player.vy = flip_velocity[player.flipped];
        }
    }

    if (JOY1_HELD & BUTTON_SELECT)
    {
        deathseq = 60;
    }
}

static inline void apply_momentum()
{
    static signed char steps[] = { 1, -1 };

    signed char player_vx_shifted;
    signed char player_vy_shifted;

    if (player.vx > 6)
    {
        player.vx = 6;
    }

    if (player.vx < -6)
    {
        player.vx = -6;
    }

    if (player.vy > 10)
    {
        player.vy = 10;
    }

    if (player.vy < -10)
    {
        player.vy = -10;
    }

    player_vx_shifted = player.vx >> 1;
    player_vy_shifted = player.vy >> 1;

    if (player.vx & 0b10000000)
    {
        player_vx_shifted |= 0b10000000;
    }

    if (player.vy & 0b10000000)
    {
        player_vy_shifted |= 0b10000000;
    }

    if (player_vx_shifted != 0x00 && is_solid_to_player(player.x + player_vx_shifted, player.y))
    {
        signed char step = steps[(player.vx >> 7) & 0x01];

        player.vx = 0;

        while (!is_solid_to_player(player.x + step, player.y))
        {
            player.x += step;
        }
    }
    else
    {
        player.x += player_vx_shifted;
    }

    if (player_vy_shifted != 0x00 && is_solid_to_player(player.x, player.y + player_vy_shifted))
    {
        signed char step = steps[(player.vy >> 7) & 0x01];

        player.vy = 0;
        while (!is_solid_to_player(player.x, player.y + step))
        {
            player.y += step;
        }
    }
    else
    {
        player.y += player_vy_shifted;
    }

    check_hazards();
}

BOOL player_onground(void)
{
    static signed char offset[] = {
        1,
        -1
    };

    return is_solid_to_player(player.x, player.y + offset[player.flipped]);
}

static inline void check_blocks(void)
{
    unsigned char i = 0;
    unsigned char player_x2 = player.x + 11;
    unsigned char player_y2 = player.y + 20;

    for (; i < current_block; ++i)
    {
        if (blocks.type[i] & BLOCK_HURT)
        {
            if (player.x < blocks.x2[i] &&
                player_x2 > blocks.x[i] &&
                player.y < blocks.y2[i] &&
                player_y2 > blocks.y[i])
            {
                deathseq = 60;
            }
        }
    }
}

static signed char gravity[] = { 1, -1 };

void player_update(void)
{
    const BOOL on_ground = player_onground();

    if (!on_ground)
    {
        player.vy += gravity[player.flipped];
    }
    else
    {
        player.vy = 0;
    }

    apply_momentum();
    check_blocks();

    if (on_ground)
    {
        if (player.vx != 0)
        {
            if (++player.framedelay >= 8)
            {
                player.framedelay = 0;
                player.frame++;

                if (player.frame >= 2)
                {
                    player.frame = 0;
                }
            }
        }
        else
        {
            player.frame = 0;
        }
    }
    else
    {
        player.frame = 1;
    }
}

static const BOOL death_visible[] = {
    TRUE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    TRUE,
    TRUE,
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    FALSE
};

static const BOOL life_visible[] = {
    TRUE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    FALSE,
    TRUE,
    FALSE,
    FALSE,
    FALSE
};

void deathsequence(void)
{
    player.visible = death_visible[deathseq >> 1];

    --deathseq;

    if (deathseq == 0)
    {
        player_respawn();
    }
}

void lifesequence(void)
{
    if (lifeseq > 0)
    {
        player.visible = life_visible[lifeseq >> 1];

        if (lifeseq > 10)
        {
            player.flipped = save.player_flipped;
        }

        --lifeseq;
    }
}
