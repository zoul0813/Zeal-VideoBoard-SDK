/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include <stdint.h>

#define CONTROLLER_B        0
#define CONTROLLER_Y        1
#define CONTROLLER_SELECT   2
#define CONTROLLER_START    3
#define CONTROLLER_UP       4
#define CONTROLLER_DOWN     5
#define CONTROLLER_LEFT     6
#define CONTROLLER_RIGHT    7
#define CONTROLLER_A        8
#define CONTROLLER_X        9
#define CONTROLLER_L        10
#define CONTROLLER_R        12

void controller_init(void);

uint16_t read_controller(uint8_t* keys);
