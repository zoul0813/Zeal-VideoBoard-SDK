/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_keyboard.h>
#include <zos_video.h>
#include <zos_time.h>
#include <zos_video.h>
#include <zvb_gfx.h>
#include "utils.h"
#include "keyboard.h"
#include "controller.h"
#include "menu.h"
#include "title.h"
#include "assets.h"
#include "game.h"

// static uint8_t play(void);
static void init(void);
static void deinit(void);
static void reset(void);
static void game_over(void);

static void draw(void);
static uint8_t input(void);
static void input_wait(uint16_t waitFor);

static uint8_t update(void);
static uint8_t check_collision(void);
static uint8_t position_in_snake(uint8_t from, uint8_t x, uint8_t y);

static void place_fruit(Point* point);

static void update_stat(void);

Snake snake;
Point fruit;
gfx_context vctx;
uint8_t controller_mode = 1;
uint8_t boost_on = 8;
uint16_t frames = 0;

/* Background colors */
const uint8_t background_palette[] = {
  0xA8, 0xAE, 0x27, 0x9E, 0x44, 0x6C
};

int main(void) {
    init(); // Initialize everything

    menu(); // Show the menu

    reset(); // Reset for game

    // Game Loop
    while(input() == 0) {
        gfx_wait_vblank(&vctx);
        ++frames;
        if(frames == MINIMUM_WAIT - snake.speed) {
            if(update() || check_collision()) {
                game_over();
                input_wait(SNES_START | SNES_SELECT | SNES_B);
                reset();
            }
            draw();
            frames = 0;
        }

        gfx_wait_end_vblank(&vctx);
    }

    deinit(); // deinitialize graphics, etc

    printf("Game complete\n");
    printf("Score: %d\n\n", snake.score);

    return 0;
}

static void game_over(void) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            gfx_tilemap_place(&vctx, TILE_APPLE, 1, j, i);
            msleep(4);
        }
    }
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

static void update_stat(void) {
    char text[10];
    sprintf(text,"%02d/%02d", snake.speed, snake.apples_to_boost);
    nprint_string(text, strlen(text), 1, HEIGHT);
    sprintf(text,"SCORE:%03d", snake.score);
    nprint_string(text, strlen(text), 10, HEIGHT);
}

static void init(void) {
    zos_err_t err = keyboard_init();
    if(err != ERR_SUCCESS) {
        printf("Failed to initialize keyboard, %d", err);
        exit(1);
    }

    err = controller_init();
    if(err != ERR_SUCCESS) {
        printf("Failed to initialize controller, %d", err);
    }
    // verify the controller is actually connected
    uint16_t test = controller_read();
    // if unconnected, we'll get back 0xFFFF (all buttons pressed)
    if(test & 0xFFFF) {
        controller_mode = 0;
    }

    /* Disable the screen to prevent artifacts from showing */
    gfx_enable_screen(0);

    err = gfx_initialize(ZVB_CTRL_VID_MODE_GFX_320_8BIT, &vctx);
    if (err) exit(1);

    /* The first color is transparent in our palette, still valid */
    extern uint8_t _snake_palette_start;
    extern uint8_t _snake_palette_end;
    const size_t snake_palette_size = &_snake_palette_end - &_snake_palette_start;
    err = gfx_palette_load(&vctx, &_snake_palette_start, snake_palette_size, 0);
    if (err) exit(1);

    err = gfx_palette_load(&vctx, background_palette, sizeof(background_palette), BACKGROUND_INDEX);
    if (err) exit(1);

    extern uint8_t _letters_palette_start;
    extern uint8_t _letters_palette_end;
    const size_t letters_palette_size = &_letters_palette_end - &_letters_palette_start;
    err = gfx_palette_load(&vctx, &_letters_palette_start, letters_palette_size, 32);
    if (err) exit(1);



    /* Load the tilesets */
    extern uint8_t _snake_tileset_end;
    extern uint8_t _snake_tileset_start;
    const size_t tileset_size = &_snake_tileset_end - &_snake_tileset_start;
    gfx_tileset_options options = {
        .compression = TILESET_COMP_NONE,
    };
    err = gfx_tileset_load(&vctx, &_snake_tileset_start, tileset_size, &options);
    if (err) exit(1);

    extern uint8_t _letters_tileset_end;
    extern uint8_t _letters_tileset_start;
    const size_t letter_tileset_size = &_letters_tileset_end - &_letters_tileset_start;
    options.compression = TILESET_COMP_4BIT;
    options.from_byte = 0x4100; // 'A' << 256
    options.pal_offset = 32;
    options.opacity = 1;
    err = gfx_tileset_load(&vctx, &_letters_tileset_start, letter_tileset_size, &options);
    if (err) exit(1);

    extern uint8_t _numbers_tileset_end;
    extern uint8_t _numbers_tileset_start;
    const size_t numbers_tileset_size = &_numbers_tileset_end - &_numbers_tileset_start;
    options.from_byte = 0x3000; // '0' << 256
    err = gfx_tileset_load(&vctx, &_numbers_tileset_start, numbers_tileset_size, &options);
    if (err) exit(1);

    /* Create two colored tiles for the background */
    gfx_tileset_add_color_tile(&vctx, BACKGROUND_TILE, BACKGROUND_INDEX);
    gfx_tileset_add_color_tile(&vctx, BACKGROUND_TILE + 1, BACKGROUND_INDEX + 1);
    /* One black tile (color 1 is black) */
    gfx_tileset_add_color_tile(&vctx, BACKGROUND_TILE + 2, BACKGROUND_INDEX + 2);

    /* Fill the layer0 with the background pattern */
    draw_background();

    gfx_enable_screen(1);
}

static void deinit(void) {
    title_hide();
    ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);
}

static void reset(void) {
    /* Disable the screen to prevent artifacts from showing */
    gfx_enable_screen(0);

    uint8_t mode = get_menu_selection();
    boost_on = 8 - (mode * 2);

    // Initialize snake
    snake.length = 2;
    snake.body[0].x = WIDTH / 2;
    snake.body[0].y = HEIGHT / 2;
    snake.body[1].x = WIDTH / 2 - 1;
    snake.body[1].y = HEIGHT / 2;
    snake.direction = DIRECTION_RIGHT;
    snake.speed = 5 * mode;
    snake.score = 0;
    snake.apples_to_boost = boost_on;

    draw_background();
    place_fruit(&fruit);

    update_stat();

    gfx_enable_screen(1);
}

static uint8_t get_direction(uint8_t former_x, uint8_t former_y, uint8_t x, uint8_t y)
{
    if (former_x == x + 1)      return DIRECTION_RIGHT;
    else if (former_x == x - 1) return DIRECTION_LEFT;
    else if (former_y == y + 1) return DIRECTION_DOWN;
    else if (former_y == y - 1) return DIRECTION_UP;

    return 0;
}

static void draw(void) {
    /* Remove deleted tile */
    const Point* p = &snake.deleted;
    gfx_tilemap_place(&vctx, TILE_TRANSPARENT, 1, p->x, p->y);

    /* Draw snake body */
    uint8_t former_x = snake.body[0].x;
    uint8_t former_y = snake.body[0].y;
    gfx_tilemap_place(&vctx, TILE_HEAD_TOP + snake.direction, 1, former_x, former_y);
    uint8_t i = 1;

    uint8_t tile = TILE_BODY_LINE;
    for (; i < snake.length - 1; i++) {
        const uint8_t x = snake.body[i].x;
        const uint8_t y = snake.body[i].y;
        const uint8_t from = get_direction(former_x, former_y, x, y);
        const uint8_t to = get_direction(snake.body[i+1].x, snake.body[i+1].y, x, y);
        switch (from) {
            case DIRECTION_UP:
                if (to == DIRECTION_DOWN) tile = TILE_BODY_COLUMN;
                else if (to == DIRECTION_LEFT) tile = TILE_BODY_TOP_LEFT;
                else if (to == DIRECTION_RIGHT) tile = TILE_BODY_TOP_RIGHT;
                break;
            case DIRECTION_RIGHT:
                if (to == DIRECTION_DOWN) tile = TILE_BODY_RIGHT_BOTTOM;
                else if (to == DIRECTION_LEFT) tile = TILE_BODY_LINE;
                else if (to == DIRECTION_UP) tile = TILE_BODY_TOP_RIGHT;
                break;
            case DIRECTION_DOWN:
                if (to == DIRECTION_UP) tile = TILE_BODY_COLUMN;
                else if (to == DIRECTION_LEFT) tile = TILE_BODY_LEFT_BOTTOM;
                else if (to == DIRECTION_RIGHT) tile = TILE_BODY_RIGHT_BOTTOM;
                break;
            case DIRECTION_LEFT:
                if (to == DIRECTION_DOWN) tile = TILE_BODY_LEFT_BOTTOM;
                else if (to == DIRECTION_RIGHT) tile = TILE_BODY_LINE;
                else if (to == DIRECTION_UP) tile = TILE_BODY_TOP_LEFT;
                break;
        }
        gfx_tilemap_place(&vctx, tile, 1, x, y);

        former_x = x;
        former_y = y;
    }
    const uint8_t x = snake.body[i].x;
    const uint8_t y = snake.body[i].y;
    const uint8_t from = get_direction(former_x, former_y, x, y);
    tile = TILE_TRANSPARENT;
    switch (from) {
        case DIRECTION_UP:
            tile = TILE_BODY_TOP;
            break;
        case DIRECTION_RIGHT:
            tile = TILE_BODY_RIGHT;
            break;
        case DIRECTION_DOWN:
            tile = TILE_BODY_BOTTOM;
            break;
        case DIRECTION_LEFT:
            tile = TILE_BODY_LEFT;
            break;
    }
    gfx_tilemap_place(&vctx, tile, 1, x, y);

    // Draw fruit
    gfx_tilemap_place(&vctx, TILE_APPLE, 1, fruit.x, fruit.y);
}

static uint8_t input(void) {
    uint16_t input = keyboard_read();
    if(controller_mode == 1) {
        input |= controller_read();
    }

    int8_t direction = -1;
    if(snake.direction != DIRECTION_DOWN && input & SNES_UP) direction = DIRECTION_UP;
    if(snake.direction != DIRECTION_UP && input & SNES_DOWN) direction = DIRECTION_DOWN;
    if(snake.direction != DIRECTION_RIGHT && input & SNES_LEFT) direction = DIRECTION_LEFT;
    if(snake.direction != DIRECTION_LEFT && input & SNES_RIGHT) direction = DIRECTION_RIGHT;
    if(input & SNES_START) return 255; // Esc/Quit

    if (direction >= 0) {
        snake.direction = direction;
    }


    return 0;
}

static void input_wait(uint16_t waitFor) {
    while(1) {
        uint16_t input = keyboard_read();
        if(controller_mode == 1) {
            input |= controller_read();
        }
        if(waitFor == 0 && input > 0) break;
        if(input & waitFor) break;
    }

    while(1) {
        uint16_t input = keyboard_read();
        if(controller_mode == 1) {
            input |= controller_read();
        }
        if(input == 0) break;
    }
}

static uint8_t update(void) {
    // Move snake
    if (snake.just_ate) {
        /* Out of screen */
        snake.deleted.x = 79;
        snake.just_ate = 0;
        snake.apples_to_boost--;
        snake.length++;
        snake.score++;
        if (snake.apples_to_boost == 0) {
            snake.apples_to_boost = boost_on;
            if(snake.speed < MAX_SPEED) {
                snake.speed++;
            }
        }
        update_stat();
    } else {
        snake.deleted = snake.body[snake.length - 1];
    }

    for (uint8_t i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }

    switch (snake.direction) {
        case DIRECTION_UP:
            snake.body[0].y--;
            break;
        case DIRECTION_DOWN:
            snake.body[0].y++;
            break;
        case DIRECTION_LEFT:
            snake.body[0].x--;
            break;
        case DIRECTION_RIGHT:
            snake.body[0].x++;
            break;
    }


    return position_in_snake(1, snake.body[0].x, snake.body[0].y);
}

static uint8_t check_collision(void) {
    // Check if snake has collided with walls or itself
    if (snake.body[0].x == WIDTH  || snake.body[0].x == 0xff ||
        snake.body[0].y == HEIGHT || snake.body[0].y == 0xff) {
        return 1;
    }

    // Check if snake has eaten fruit
    if (snake.body[0].x == fruit.x && snake.body[0].y == fruit.y) {
        if (snake.length != 0xff)
            snake.just_ate = 1;
        do {
            place_fruit(&fruit);
        } while (position_in_snake(0, fruit.x, fruit.y));
    }
    return 0;
}

static uint8_t position_in_snake(uint8_t from, uint8_t x, uint8_t y) {
    for (uint8_t i = from; i < snake.length; i++) {
        Point* p = &snake.body[i];
        if (p->x == x && p->y == y)
            return 1;
    }
    return 0;
}

static void place_fruit(Point* point) {
    point->x = rand8() % WIDTH;
    point->y = rand8() % HEIGHT;
}