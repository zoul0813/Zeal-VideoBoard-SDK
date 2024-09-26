#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_keyboard.h>
#include "menu.h"
#include "utils.h"
#include "controller.h"
#include "keyboard.h"
#include "game.h"
#include "title.h"

extern gfx_context vctx;
uint8_t menu_current_selection = 0;

uint8_t get_menu_selection(void) {
  return menu_current_selection;
}

uint16_t last_input = 0;
uint8_t process_menu(void)
{
  uint16_t input = keyboard_read();
  if (controller_mode) {
    // OR the two inputs to make a single input
    input |= controller_read();
  }

  if(input == last_input) return 0; // nothing to do here?
  last_input = input; // update last input
  if(input == 0) return 0; // no input

  // TODO: handle input to change menu selection
  if(input & SNES_UP) {
    if(menu_current_selection == 0) {
      menu_current_selection = 4;
    }
    menu_current_selection--;
    return 1;
  }

  if(input & SNES_DOWN) {
    menu_current_selection++;
    if(menu_current_selection > 3) {
      menu_current_selection = 0;
    }
    return 1;
  }

  if(input & (SNES_RIGHT | SNES_START)) {
    return 255;
  }

  return 0;
}

#define MENU_START_Y  5
void draw_menu(void)
{
  // char text[7];

  // print_string("ZEAL SNAKE", WIDTH / 2 - 5, 3);

  // Game Modes
  print_string("EASY", WIDTH / 2 - 3, MENU_START_Y);
  print_string("MEDIUM", WIDTH / 2 - 3, MENU_START_Y+1);
  print_string("HARD", WIDTH / 2 - 3, MENU_START_Y+2);

  // Quit
  print_string("QUIT", WIDTH / 2 - 3, MENU_START_Y+4);

  uint8_t menu_y = MENU_START_Y + menu_current_selection;
  if(menu_current_selection == 3) menu_y++;

  for(uint8_t i = 0; i < 5; i++) {
    if(i == menu_y) continue;
    gfx_tilemap_place(&vctx, TILE_TRANSPARENT, 1, WIDTH / 2 - 5, MENU_START_Y + i);
  }

  gfx_tilemap_place(&vctx, TILE_APPLE, 1, WIDTH / 2 - 5, menu_y);


#ifdef DEBUG
  char text[5];
  sprintf(text, "%05d", menu_current_selection);
  nprint_string(text, 5, WIDTH / 2 - 3, 12);

  sprintf(text, "%05d", menu_y);
  nprint_string(text, 5, WIDTH / 2 - 3, 13);
#endif
}