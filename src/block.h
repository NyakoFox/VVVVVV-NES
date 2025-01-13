#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_AMOUNT 64

extern unsigned char current_block;

#define BLOCK_INVALID 0x00
#define BLOCK_HURT    0x01

typedef struct {
    unsigned char x[BLOCK_AMOUNT];
    unsigned char y[BLOCK_AMOUNT];
    unsigned char x2[BLOCK_AMOUNT];
    unsigned char y2[BLOCK_AMOUNT];
    unsigned char type[BLOCK_AMOUNT];
} block_t;

extern block_t blocks;

void create_block(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char type);

#endif