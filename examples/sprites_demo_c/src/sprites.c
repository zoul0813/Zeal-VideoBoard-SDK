/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_keyboard.h>
#include <zos_time.h>
#include <zvb_gfx.h>
#include "sprites.h"

#define BACKGROUND_INDEX    16
#define BACKGROUND_TILE     32

static void init_game(void);
static void draw_background(void);
static void draw();
static void update(uint16_t frames);

gfx_context vctx;
gfx_sprite sprite1;

int main(void) {
    init_game();

    uint16_t frames = 0;
    while (1) {
        frames++;
        update(frames);
        if(frames % 90 == 0) {
            draw();
        }
    }
    // return 0; // unreachable
}

static void draw_background(void) {
    uint8_t line[WIDTH + 1];
    /* Tile 15 is a transparent tile, fill layer1 with it */
    uint8_t layer1[WIDTH];

    for (int i = 0; i < WIDTH; i++) {
        layer1[i] = TILE_TRANSPARENT;
        line[i] = BACKGROUND_TILE + (i&1);
    }
    line[WIDTH] = BACKGROUND_TILE;

    for (uint8_t i = 0; i < HEIGHT; i++) {
        gfx_tilemap_load(&vctx, line + (i & 1), WIDTH, 0, 0, i);
        gfx_tilemap_load(&vctx, layer1, WIDTH, 1, 0, i);
    }
    /* Set the layer0 last line to dark green */
    for (uint8_t i = 0; i < WIDTH; i++) {
        line[i] = BACKGROUND_TILE + 2;
    }
    gfx_tilemap_load(&vctx, line, WIDTH, 0, 0, HEIGHT);
    /* set the layer1 last line to transparent */
    gfx_tilemap_load(&vctx, layer1, WIDTH, 1, 0, HEIGHT);
}

static void init_game(void) {
    /* Disable the screen to prevent artifacts from showing */
    gfx_enable_screen(0);

    gfx_error err = gfx_initialize(ZVB_CTRL_VID_MODE_GFX_320_8BIT, &vctx);
    if (err) exit(1);

    extern uint8_t _tank_palette_end;
    extern uint8_t _tank_palette_start;
    const size_t palette_size = &_tank_palette_end - &_tank_palette_start;

    err = gfx_palette_load(&vctx, &_tank_palette_start, palette_size, 0);
    if (err) exit(1);

    extern uint8_t _tank_sprite_end;
    extern uint8_t _tank_sprite_start;
    const size_t sprite_size = &_tank_sprite_end - &_tank_sprite_start;
    gfx_tileset_options options = {
        .compression = TILESET_COMP_NONE,
    };
    err = gfx_tileset_load(&vctx, &_tank_sprite_start, sprite_size, &options);
    if (err) exit(1);

    gfx_tileset_add_color_tile(&vctx, BACKGROUND_TILE, BACKGROUND_INDEX);
    gfx_tileset_add_color_tile(&vctx, BACKGROUND_TILE + 1, BACKGROUND_INDEX + 1);
    /* One black tile (color 1 is black) */
    gfx_tileset_add_color_tile(&vctx, BACKGROUND_TILE + 2, BACKGROUND_INDEX + 2);

    draw_background();

    sprite1.flags = SPRITE_NONE;
    gfx_sprite_set_tile(&vctx, 0, 1);

    gfx_enable_screen(1);
}

static void draw() {
    static uint16_t x = 16;
    static uint16_t y = 16;
    static int8_t xd = 1;
    static int8_t yd = 1;
    /* Wait for v-blank */
    gfx_wait_vblank(&vctx);


    x += xd;
    y += yd;

    if(x > 320) { // 320 - 16
        xd = -1;
    }
    if(x <= 16) {
        xd = 1;
    }

    if(y > 240) { // 240 - (16 * 2)
        yd = -1;
    }
    if(y <= 16) {
        yd = 1;
    }

    sprite1.x = x;
    sprite1.y = y;

    if(xd < 0) {
        sprite1.flags = SPRITE_NONE;
    } else {
        sprite1.flags = SPRITE_FLIP_X;
    }

    uint8_t err = gfx_sprite_render(&vctx, 0, &sprite1);
    if(err != 0) {
        printf("graphics error: %d", err);
        exit(1);
    }
}

static void update(uint16_t frames) {
    if(frames % 512 == 0) {
        sprite1.tile ^= 1;

    }
}

/**
 * @brief Workaround to include a binary file in the program
 */
void _tank_palette() {
    __asm
__tank_palette_start:
    .incbin "assets/tank.ztp"
__tank_palette_end:
    __endasm;
}

/**
 * @brief Workaround to include a binary file in the program
 */
void _tank_sprite() {
    __asm
__tank_sprite_start:
    .incbin "assets/tank.zts"
__tank_sprite_end:
    __endasm;
}
