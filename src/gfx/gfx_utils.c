/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_gfx.h"
#include "gfx.h"

void memset_vram(void* ptr, int a, uint16_t size) __naked
{
    (void) ptr;
    (void) a;
    (void) size;
    __asm__(
    "    ld a, e\n" \
    "    pop de\n" \
    "    pop bc\n" \
    "    push de\n" \
    "    ld e, a\n" \
    "    ; BC has the size now\n" \
    "_memset_vram_loop:\n" \
    "    ld a, b\n" \
    "    or c\n" \
    "    ret z\n" \
    "    ld (hl), e\n" \
    "    inc hl\n" \
    "    dec bc\n" \
    "    jp _memset_vram_loop\n" \
    );
}

void* mem_cpy(void* dst, const void* src, size_t size)
{
    uint8_t* d       = dst;
    const uint8_t* s = src;
    while (size--) *d++ = *s++;
    return dst;
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
        mem_cpy(dst, src, size);
    }
}
