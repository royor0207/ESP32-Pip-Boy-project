idf_component_register(SRCS "sensor_display.c"
                       INCLUDE_DIRS "."
                       REQUIRES 
                        lvgl__lvgl 
                        ui   
                        hardware_init)
