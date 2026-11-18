#pragma once

#include "control_link/ControlLink.hpp"

// From PCB_BrainBoard.PcbDoc (CDFR2027-PAMIs-ELEC), ESP32-S3-WROOM-1 module pin in brackets
constexpr control_link_pins_t CONTROL_LINK_PINS = {
    GPIO_NUM_12, // sclk      [20] SPI_CB_CLK
    GPIO_NUM_13, // mosi      [21] SPI_CB_MOSI
    GPIO_NUM_11, // miso      [19] SPI_CB_MISO
    GPIO_NUM_10, // cs        [18] SPI_CB_CS
    GPIO_NUM_9,  // handshake [17] SPI_CB_Handshake
};

constexpr gpio_num_t LED_PIN = GPIO_NUM_2; // TODO: placeholder, real GPIO from the BrainBoard schematic
