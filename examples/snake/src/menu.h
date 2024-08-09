/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include <stdint.h>

#define MENU_MODE_EASY 0
#define MENU_MODE_MEDIUM 1
#define MENU_MODE_HARD 2
#define MENU_QUIT 3

uint8_t get_menu_selection(void);
uint8_t process_menu(void);
void draw_menu(void);