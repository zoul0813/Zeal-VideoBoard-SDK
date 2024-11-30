/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <zvb_gfx.h>

#define MINIMUM_WAIT    20
#define MAX_SPEED       10
#define BOOST_ON        8

#define WIDTH 20
/* Keep one line at the bottom for the score */
#define HEIGHT 14
#define SNAKE_MAX_LENGTH 256

#define BACKGROUND_INDEX    16
#define BACKGROUND_TILE     32

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

#define DIRECTION_UP    0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN  2
#define DIRECTION_LEFT  3

#define KEY_UP    0
#define KEY_RIGHT 1
#define KEY_DOWN  2
#define KEY_LEFT  3

#define TILE_HEAD_TOP           0
#define TILE_HEAD_RIGHT         1
#define TILE_HEAD_BOTTOM        2
#define TILE_HEAD_LEFT          3
#define TILE_BODY_TOP_LEFT      4
#define TILE_BODY_TOP_RIGHT     5
#define TILE_BODY_BOTTOM        6
#define TILE_APPLE              7
#define TILE_BODY_RIGHT_BOTTOM  8
#define TILE_BODY_LEFT          9
#define TILE_BODY_LEFT_BOTTOM   10
#define TILE_BODY_RIGHT         11
#define TILE_BODY_LINE          12
#define TILE_BODY_COLUMN        13
#define TILE_BODY_TOP           14
#define TILE_TRANSPARENT        15

typedef struct {
    Point   body[SNAKE_MAX_LENGTH];
    Point   deleted;
    uint8_t length;
    uint8_t direction;
    uint8_t just_ate;
    uint16_t score;
    /* 0: slowest, 20: fastest */
    uint8_t speed;
    uint8_t apples_to_boost;
} Snake;


extern Snake snake;
extern Point fruit;
extern gfx_context vctx;
extern uint8_t controller_mode;