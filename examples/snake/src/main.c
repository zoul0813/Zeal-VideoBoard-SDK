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
#include "controller.h"
#include "menu.h"
#include "title.h"
#include "assets.h"
#include "game.h"

static uint8_t play(void);
static uint8_t menu(void);
static void init(void);
static void init_game(void);
static void wait(void);
static void draw(void);
static uint8_t input(void);
static uint8_t update(void);
static uint8_t check_collision(void);
static void place_fruit(Point* point);
static void end_game(void);
static void quit_game(void);
static uint8_t position_in_snake(uint8_t from, uint8_t x, uint8_t y);
static void update_stat(void);

Snake snake;
Point fruit;
int controller_mode;
gfx_context vctx;
int controller_mode;
uint8_t boost_on = 8;

/* Background colors */
const uint8_t background_palette[] = {
  0xA8, 0xAE, 0x27, 0x9E, 0x44, 0x6C
};

int main(int argc, char** argv) {
    if (argc == 1){
        char* param = strtok(argv[0], " ");
        if (param && (strcmp(param, "-c") == 0)) {
            controller_mode = 1;
            controller_init();
        }
    }
    init();
    menu();
    return play();
}

static uint8_t menu(void) {
    static uint8_t frames = 0;
    play_title();
    draw_menu();
    while(1) {
        gfx_wait_vblank(&vctx);
        ++frames;
        if(frames >= 180) {
            frames = 0;
            title_flip_head();
        }

        uint8_t state = process_menu();
        if(state > 0) {
            draw_menu();
        }
        if(state == 255) {
            uint8_t mode = get_menu_selection();
            if(mode == MENU_QUIT) quit_game();
            break;
        }
    }
    hide_title();
    return 0;
}

static uint8_t play(void) {
    init_game();
    update_stat();

    uint8_t state = 0;
    // initialize frame counter for FPS
    uint8_t frames = 0;
    while (1) {
        state = input();
        if(state != 0) quit_game();
        gfx_wait_vblank(&vctx);
        ++frames;
        if(frames >= MINIMUM_WAIT - snake.speed) {
            if(update() || check_collision())
                break;
            frames = 0;
            draw();
        }
        gfx_wait_end_vblank(&vctx);
    }
    end_game();

    uint8_t key[8];
    uint16_t size = 8;
    /* Flush the keyboard fifo */
    while (size) {
        read(DEV_STDIN, key, &size);
    }

    /* Wait for a key from the user now */
    do {
        size = 1;
        read(DEV_STDIN, &key, &size);
    } while (size != 1);

    return play();
}

static void quit_game(void) {
    ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);
    // TODO: clear screen and sprites!
    exit(0);
}

static void end_game(void) {
    // /* Change the colors of the snake to greyscale */
    // const uint8_t palette_from = 6;
    // const uint16_t colors[] = { 0xae, 0x73, 0xef, 0x7b, 0x10, 0x84 };

    // gfx_palette_load(&vctx, colors, 3, palette_from);

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
    /* Initialize the keyboard by setting it to raw and non-blocking */
    void* arg = (void*) (KB_READ_NON_BLOCK | KB_MODE_RAW);
    ioctl(DEV_STDIN, KB_CMD_SET_MODE, arg);

    /* Disable the screen to prevent artifacts from showing */
    gfx_enable_screen(0);

    gfx_error err = gfx_initialize(ZVB_CTRL_VID_MODE_GFX_320_8BIT, &vctx);
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

static void init_game(void) {
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
    snake.former_direction = DIRECTION_RIGHT;
    snake.speed = 5 * mode;
    snake.score = 0;
    snake.apples_to_boost = boost_on;

    draw_background();
    place_fruit(&fruit);

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

static int8_t check_key(uint8_t key) {
    switch (key) {
        case KB_UP_ARROW:
            if (snake.direction != DIRECTION_DOWN)
                return DIRECTION_UP;
            break;
        case KB_DOWN_ARROW:
            if (snake.direction != DIRECTION_UP)
                return  DIRECTION_DOWN;
            break;
        case KB_LEFT_ARROW:
            if (snake.direction != DIRECTION_RIGHT)
                return DIRECTION_LEFT;
            break;
        case KB_RIGHT_ARROW:
            if (snake.direction != DIRECTION_LEFT)
                return DIRECTION_RIGHT;
            break;
        case KB_ESC:
            return -2;
    }

    return -1;
}

static uint8_t keys[32];

static uint8_t input(void) {
    uint16_t size = 32;
    const int8_t last_direction = snake.former_direction;
    int8_t chosen = -1;
    uint8_t exit = 0;

    while (!exit) {
        /* Give priority to the keyboard, if no key is detected, check the external controller */
        read(DEV_STDIN, keys, &size);
        /* Since we are in non-blocking mode, `read` syscall can return 0 */
        if (size == 0 && controller_mode) {
            size = read_controller(keys);
            exit = 1;
        }

        if (size == 0)
            break;

        for (uint8_t i = 0; i < size; i++) {
            if (keys[i] == KB_RELEASED) {
                i++;
            } else {
                int8_t direction = check_key(keys[i]);
                /* Prioritize direction change */
                if (direction >= 0 && direction != last_direction) {
                    chosen = direction;
                    break;
                } else {
                    if(direction == -2) {
                        return 255;
                    }
                }
            }
        }
    }

    if (chosen != -1) {
        snake.direction = chosen;
        snake.former_direction = chosen;
    }

    return 0;
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