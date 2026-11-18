#pragma once

#include "control_link/ControlLink.hpp"

// TODO: real GPIOs from the BrainBoard schematic (nets SPI_CB_CLK, SPI_CB_MOSI, SPI_CB_MISO, SPI_CB_CS, SPI_CB_Handshake)
constexpr control_link_pins_t CONTROL_LINK_PINS = {
    GPIO_NUM_12, // sclk      TODO: placeholder
    GPIO_NUM_11, // mosi      TODO: placeholder
    GPIO_NUM_13, // miso      TODO: placeholder
    GPIO_NUM_10, // cs        TODO: placeholder
    GPIO_NUM_14, // handshake TODO: placeholder
};
