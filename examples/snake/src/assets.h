#include <stdio.h>
#include <stdint.h>

// /**
//  * @brief Palette for the graphics tiles including the snake, the apple and the background
//  */
// const uint8_t assets_palette[] = {
//   0x1f, 0xdc, 0x00, 0x00, 0xc0, 0x08, 0xa1, 0x8a, 0x62, 0x53, 0x40, 0xda,
//   0x9b, 0x4b, 0xdf, 0x5b, 0xff, 0x5b, 0xc3, 0x05, 0x7f, 0x6c, 0xdc, 0x7c,
//   0x83, 0x66, 0x85, 0x5e, 0xbf, 0x9d, 0xbe, 0xf7,
//   /* Background colors */
//   0xa8, 0xae, 0x27, 0x9e, 0x44, 0x6c
// };

// /**
//  * @brief Palette for the text, including numbers and letters
//  */
// const uint8_t letters_palette[] = {
//   0x00, 0x00, 0x83, 0xa8, 0xcb, 0xf1, 0xc7, 0xfc, 0x52, 0xff, 0xff, 0xff
// };

/**
 * @brief Workaround to include a binary file in the program
 */

void _snake_palette(void) {
    __asm
__snake_palette_start:
    .incbin "assets/snake_tileset.ztp"
__snake_palette_end:
    __endasm;
}

void _snake_tileset(void) {
    __asm
__snake_tileset_start:
    .incbin "assets/snake_tileset.zts"
__snake_tileset_end:
    __endasm;
}

void _letters_palette(void) {
    __asm
__letters_palette_start:
    .incbin "assets/letters.ztp"
__letters_palette_end:
    __endasm;
}

void _letters_tileset(void) {
    __asm
__letters_tileset_start:
    .incbin "assets/letters.zts"
__letters_tileset_end:
    __endasm;
}

void _numbers_tileset(void) {
    __asm
__numbers_tileset_start:
    .incbin "assets/numbers.zts"
__numbers_tileset_end:
    __endasm;
}