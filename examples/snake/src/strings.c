#include <string.h>
#include "strings.h"
#include "snake.h"

void print_string(const char* str, uint8_t x, uint8_t y)
{
    gfx_tilemap_load(&vctx, str, strlen(str), 1, x, y);
}

void nprint_string(const char* str, uint8_t len, uint8_t x, uint8_t y)
{
    gfx_tilemap_load(&vctx, str, len, 1, x, y);
}
