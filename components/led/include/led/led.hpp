#pragma once

#include "driver/gpio.h"

// Starts a task that blinks the LED on `pin` forever: the "hello world" of this project
bool startLedBlink(gpio_num_t pin);
