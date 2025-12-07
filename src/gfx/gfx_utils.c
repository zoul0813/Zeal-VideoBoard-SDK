/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "zvb_gfx.h"
#include "gfx.h"

void memset_vram(void* ptr, int a, uint16_t size) __naked
{
    (void) ptr;
    (void) a;
    (void) size;
__asm
    ld a, e
    pop de
    pop bc
    push de
    ld e, a
    ; BC has the size now
_memset_vram_loop:
    ld a, b
    or c
    ret z
    ld (hl), e
    inc hl
    dec bc
    jp _memset_vram_loop
__endasm;
}

void memaddcpy(uint8_t* dst, uint8_t* src, size_t size, uint8_t opacity, uint8_t offset)
{
    if (offset) {
        while (size) {
            uint8_t byte = *src + offset;
            if (opacity && byte == offset) {
                byte = 0;
            }
            *dst = byte;
            src++;
            dst++;
            size--;
        }
    } else {
        memcpy(dst, src, size);
    }
}
