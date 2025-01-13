#include "block.h"

#include "game.h"

block_t blocks;

unsigned char current_block = 0;

void create_block(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char type)
{
    blocks.x[current_block] = x;
    blocks.y[current_block] = y;
    blocks.x2[current_block] = x + width;
    blocks.y2[current_block] = y + height;
    blocks.type[current_block] = type;

    ++current_block;
};

void clear_blocks(void)
{
    current_block = 0;
    // Currently, no need to clear the actual data -- current_block
    // is used to determine how many blocks are active
}
