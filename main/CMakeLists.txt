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
        ESP-IDF-NVSExtensions
        ESP-IDF-LowPassFilter
        ESP-IDF-Battery
        ESP-IDF-BuzzerAudio
)

target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++26)