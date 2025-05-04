idf_component_register(SRCS "hardware_init.c" "music_player.c" "aht10.c" "encoder.c" "max30102.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        driver 
                        screen_switcher)
