# CRC library source files
set(CRC_SOURCES
    crc_initialize.c
    crc_reset.c
    crc_update.c
)

zvb_add_library(zvb_crc "${CRC_SOURCES}")
zvb_export_sources(ZVB_CRC_SOURCES "${CRC_SOURCES}")
