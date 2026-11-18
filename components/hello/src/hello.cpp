#include "hello/hello.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "hello";

static const int PERIOD_MS = 1000;

static void helloTask(void*) {
    int count = 0;
    while (true) {
        ESP_LOGI(TAG, "hello world %d", count);
        count++;
        vTaskDelay(pdMS_TO_TICKS(PERIOD_MS));
    }
}

bool startHello() {
    if (xTaskCreate(helloTask, "hello", 2048, nullptr, 1, nullptr) != pdPASS) {
        ESP_LOGE(TAG, "cannot create task");
        return false;
    }
    return true;
}
