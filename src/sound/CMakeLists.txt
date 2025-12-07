# Sound library source files
set(SOUND_SOURCES
    sound_initialize.c
    sound_reset.c
    sound_set_voices.c
    sound_set_voices_vol.c
    sound_set_hold.c
    sound_get_hold.c
    sound_set_channels.c
    sound_set_volume.c
    sound_set_volumes.c
    sound_play_samples.c
)

zvb_add_library(zvb_sound "${SOUND_SOURCES}")
zvb_export_sources(ZVB_SOUND_SOURCES "${SOUND_SOURCES}")
