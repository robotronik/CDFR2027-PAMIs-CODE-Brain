#include "board.hpp"
#include "esp_log.h"
#include "led/led.hpp"

static const char* TAG = "main";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "BrainBoard starting");

    if (!startLedBlink(LED_PIN)) {
        ESP_LOGE(TAG, "LED failed to start");
    }

    static ControlLink controlLink;
    if (!controlLink.start(CONTROL_LINK_PINS)) {
        ESP_LOGE(TAG, "control link failed to start");
    }
}
