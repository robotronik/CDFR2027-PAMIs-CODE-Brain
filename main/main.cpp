#include "board.hpp"
#include "esp_log.h"
#include "hello/hello.hpp"

static const char* TAG = "main";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "BrainBoard starting");

    if (!startHello()) {
        ESP_LOGE(TAG, "hello failed to start");
    }

    static ControlLink controlLink;
    if (!controlLink.start(CONTROL_LINK_PINS)) {
        ESP_LOGE(TAG, "control link failed to start");
    }
}
