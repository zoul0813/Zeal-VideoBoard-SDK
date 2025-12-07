/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_hardware.h"
#include "zvb_dma.h"

dma_error zvb_dma_set_read_virt(zvb_dma_descriptor_t* desc, void* ptr) {
    uint32_t addr = zvb_dma_virt_to_phys(ptr);
    return zvb_dma_set_read(desc, addr);
}
