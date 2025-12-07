/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_hardware.h"
#include "zvb_dma.h"

uint32_t zvb_dma_virt_to_phys(void* ptr) __naked {
    (void*)ptr;
    __asm__ (
        // Prepare the lowest bits
        "ld e, l\n"
        "ld a, h\n"
        "and #0x3f\n"
        "ld d, a\n"
        // Get the page value
        "ld a, h\n"
        "in a, (#0xF0)\n"
        "rrca\n"
        "rrca\n"
        // Put the high bits in D
        "ld h, a\n"
        "and #0xc0\n"
        "or d\n"
        "ld d, a\n"
        // Put the lowest bits in L
        "ld a, h\n"
        "and #0x3f\n"
        "ld l, a\n"
        "ld h, #0\n"
        "ret\n"
    );
}
