idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES  
                        spiffs 
                        esp_lcd 
                       REQUIRES 
                        lvgl
                        screen_switcher
                        wifi_connect
                        hardware_init
                        time_display
                        setting_display
                        ui)


# add_compile_definitions (LV_LVGL_H_INCLUDE_SIMPLE)