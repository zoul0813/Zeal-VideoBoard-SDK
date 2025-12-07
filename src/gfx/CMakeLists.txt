# GFX library source files
set(GFX_SOURCES
    gfx_utils.c
    gfx_initialize.c
    gfx_enable_screen.c
    gfx_palette_load.c
    gfx_wait_vblank.c
    gfx_tilemap_load.c
    gfx_tilemap_place.c
    tileset/load_bitmap.c
    tileset/load_2bit.c
    tileset/load_nibble.c
    tileset/load_rle.c
    gfx_tileset_load.c
    gfx_tileset_add_color_tile.c
    gfx_sprite_render.c
    gfx_sprite_render_array.c
    gfx_sprite_set_x.c
    gfx_sprite_set_y.c
    gfx_sprite_set_tile.c
    gfx_sprite_set_flags.c
)

zvb_add_library(zvb_gfx "${GFX_SOURCES}")
zvb_export_sources(ZVB_GFX_SOURCES "${GFX_SOURCES}")
