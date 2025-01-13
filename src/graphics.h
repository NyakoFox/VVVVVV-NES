#ifndef GRAPHICS_H
#define GRAPHICS_H

unsigned char to_letter(char c);

void upload_nametable(const unsigned char* data);
void write_text(const char* text, unsigned char x, unsigned char y);

void display_sprite(unsigned char tile_index, unsigned char x, unsigned char y, unsigned char attributes);
void display_player(void);
void display_crewmate(unsigned char x, unsigned char y, unsigned char frame, unsigned char mood, unsigned char direction, unsigned char flipped, unsigned char palette);

void graphics_begin(void);
void graphics_end(void);

#endif