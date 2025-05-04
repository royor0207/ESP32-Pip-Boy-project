idf_component_register(SRCS "screen_switcher.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        lvgl__lvgl
                        ui
                        time_display
                        music_screen
                        sensor_display
                        tetris
                        hardware_init
                        setting_display
                        )

