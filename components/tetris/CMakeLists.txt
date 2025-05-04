idf_component_register(SRCS "tetris_game.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        lvgl__lvgl
                        hardware_init
                        ui)
