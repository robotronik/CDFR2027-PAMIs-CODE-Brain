#pragma once

#include <stdint.h>

#include "control_link/protocol.hpp"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

struct control_link_pins_t {
        gpio_num_t sclk;
        gpio_num_t mosi;
        gpio_num_t miso;
        gpio_num_t cs;
        gpio_num_t handshake;
};

// SPI master link to the ControlBoard. Must outlive its task, so keep it static.
// Commands are queued and sent on the next exchange; they return false if the queue is full.
class ControlLink {
    public:
        bool start(const control_link_pins_t& pins);

        bool stop();
        // x, y in mm, theta in rad (counter-clockwise)
        bool setTargetPos(float x, float y, float theta);
        // id < 4, angleDeg <= 180, or the ControlBoard answers STATUS_BAD_ARGUMENT
        bool setServo(uint8_t id, uint8_t angleDeg);
        // Latest pose, status and flags from the ControlBoard; false before the first reply
        bool getLastReply(reply_t* reply);

    private:
        static void taskEntry(void* self);
        static void onHandshake(void* self);
        void run();
        void waitForControlBoard();

        control_link_pins_t pins;
        spi_device_handle_t device = nullptr;
        SemaphoreHandle_t ready = nullptr;
        QueueHandle_t commands = nullptr;
        QueueHandle_t lastReply = nullptr;
        WORD_ALIGNED_ATTR uint8_t txFrame[FRAME_LEN];
        WORD_ALIGNED_ATTR uint8_t rxFrame[FRAME_LEN];
};
