#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_keyboard.h>
#include "menu.h"
#include "utils.h"
#include "controller.h"
#include "game.h"
#include "title.h"

extern gfx_context vctx;
uint8_t menu_current_selection = 0;
static uint8_t keys[32];
static uint8_t last_key = 0;

uint8_t get_menu_selection(void) {
  return menu_current_selection;
}

uint8_t process_menu(void)
{
  uint16_t size = 32;
  uint8_t exit = 0;

  while (!exit)
  {
    read(DEV_STDIN, keys, &size);
    if (size == 0 && controller_mode) {
      size = read_controller(keys);
      exit = 1;
    }

    if (size == 0) {
      last_key = 0;
      break;
    }

    for (uint8_t i = 0; i < size; i++) {
      if (keys[i] == KB_RELEASED) {
        i++;
      } else {
        switch(keys[i]) {
          case KB_UP_ARROW:
            if(last_key == KB_UP_ARROW) continue;
            last_key = KB_UP_ARROW;
            if(menu_current_selection == 0) {
              menu_current_selection = 4;
            }
            menu_current_selection--;
            return 1;
          case KB_DOWN_ARROW:
            if(last_key == KB_DOWN_ARROW) continue;
            last_key = KB_DOWN_ARROW;
            menu_current_selection++;
            if(menu_current_selection > 3) {
              menu_current_selection = 0;
            }
            return 1;
          case KB_RIGHT_ARROW:
          case KB_KEY_ENTER:
          case KB_KEY_SPACE:
            if(last_key == KB_RIGHT_ARROW) continue;
            last_key = KB_RIGHT_ARROW;
            return 255;
        }
      }
    }
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