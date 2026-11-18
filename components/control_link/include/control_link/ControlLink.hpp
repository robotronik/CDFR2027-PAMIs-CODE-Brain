#pragma once

#include <stdint.h>

#include "control_link/protocol.hpp"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

struct control_link_pins_t {
        gpio_num_t sclk;
        gpio_num_t mosi;
        gpio_num_t miso;
        gpio_num_t cs;
        gpio_num_t handshake;
};

// SPI master link to the ControlBoard. Must outlive its task, so keep it static.
class ControlLink {
    public:
        bool start(const control_link_pins_t& pins);

    private:
        static void taskEntry(void* self);
        static void onHandshake(void* self);
        void run();
        void waitForControlBoard();

        control_link_pins_t pins;
        spi_device_handle_t device = nullptr;
        SemaphoreHandle_t ready = nullptr;
        WORD_ALIGNED_ATTR uint8_t txFrame[FRAME_LEN];
        WORD_ALIGNED_ATTR uint8_t rxFrame[FRAME_LEN];
};
