#pragma once

#include <stdint.h>

void keyboard_init(void);
void keyboard_flush(void);
void keyboard_wait(uint16_t key);
uint16_t keyboard_read(void);
uint8_t keyboard_pressed(uint16_t key);