set(LONLYLIB_SOURCES
ui.c
eez-flow.cpp
images.c
screens.c
styles.c
actions.c
theme_pipboy.c
ui_font_share_tech_regular_20.c
ui_font_share_tech_regular_40.c
ui_font_share_tech_regular_80.c
ui_image_scanline.c
)
    
# Resister components
idf_component_register(
    SRCS
        ${LONLYLIB_SOURCES}
    INCLUDE_DIRS "."
    REQUIRES
        lvgl__lvgl
        screen_switcher
        sensor_display
        music_screen
        time_display
        tetris
        hardware_init) 
add_compile_definitions (LV_LVGL_H_INCLUDE_SIMPLE)
