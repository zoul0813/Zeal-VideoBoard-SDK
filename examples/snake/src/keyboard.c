#include <stdio.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_keyboard.h>
#include "keyboard.h"
#include "controller.h"

static uint16_t keys = 0;
static uint8_t key_buffer[32];

void keyboard_flush(void) {
  /* Flush the keyboard fifo */
  uint8_t size = 8;
  while (size) {
    read(DEV_STDIN, key_buffer, &size);
  }
}

void keyboard_init(void) {
  /* Initialize the keyboard by setting it to raw and non-blocking */
  void* arg = (void*) (KB_READ_NON_BLOCK | KB_MODE_RAW);
  ioctl(DEV_STDIN, KB_CMD_SET_MODE, arg);
}

uint16_t keyboard_read(void) {
  uint16_t size = 32;
  keys = 0;
  while(1) {
    read(DEV_STDIN, key_buffer, &size);

    if (size == 0) return keys;

    for (uint8_t i = 0; i < size; i++) {
      if (key_buffer[i] == KB_RELEASED) {
        i++;
      } else {
        switch(key_buffer[i]) {
          case KB_KEY_W:
          case KB_UP_ARROW: keys |= SNES_UP; return keys;

          case KB_KEY_S:
          case KB_DOWN_ARROW: keys |= SNES_DOWN; return keys;

          case KB_KEY_A:
          case KB_LEFT_ARROW: keys |= SNES_LEFT; return keys;

          case KB_KEY_D:
          case KB_RIGHT_ARROW: keys |= SNES_RIGHT; return keys;

          case KB_KEY_ENTER: keys |= SNES_START; return keys;
          case KB_KEY_SPACE: keys |= SNES_SELECT; return keys;
        }
      }
    }
  }

  return keys;
}

uint8_t keyboard_pressed(uint16_t button) {
    return keys & button;
}

void keyboard_wait(uint16_t key) {
  while(1) {
      uint16_t buttons = keyboard_read();
      if(!key && buttons) return; // any key
      if(buttons & key) return; // match
  }
}