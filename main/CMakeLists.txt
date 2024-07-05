if(NOT IDF_TARGET STREQUAL "esp32")
    idf_component_register(SRCS "uart_dma_main.c"
    INCLUDE_DIRS ".")
else()
    message(FATAL_ERROR "UHCI not avaliable on ESP32")
endif()
