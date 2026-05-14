file(GLOB_RECURSE pizda_sources
    "*.*"
)

idf_component_register(
    SRCS ${pizda_sources}
    INCLUDE_DIRS "."
    REQUIRES
        esp_driver_ledc
        nvs_flash
        esp_adc
        YOBA
)

target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++26)