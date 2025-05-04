idf_component_register(SRCS "setting_display.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        lvgl__lvgl 
                        music_screen 
                        hardware_init 
                        ui)
