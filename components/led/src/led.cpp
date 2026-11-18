#include "led/led.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "led";

static const int BLINK_PERIOD_MS = 500;

// Set once by startLedBlink, then only read by the task
static gpio_num_t ledPin;

static void ledTask(void*) {
    bool on = false;
    while (true) {
        on = !on;
        gpio_set_level(ledPin, on);
        vTaskDelay(pdMS_TO_TICKS(BLINK_PERIOD_MS));
    }
}

bool startLedBlink(gpio_num_t pin) {
    ledPin = pin;

    esp_err_t err = gpio_reset_pin(pin);
    if (err == ESP_OK) {
        err = gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "cannot set GPIO %d as output: %s", pin, esp_err_to_name(err));
        return false;
    }

    if (xTaskCreate(ledTask, "led", 2048, nullptr, 1, nullptr) != pdPASS) {
        ESP_LOGE(TAG, "cannot create task");
        return false;
    }
    ESP_LOGI(TAG, "blinking GPIO %d", pin);
    return true;
}
