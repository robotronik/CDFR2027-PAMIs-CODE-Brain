#include "control_link/ControlLink.hpp"

#include "esp_log.h"
#include "freertos/task.h"

static const char* TAG = "control_link";

// The ControlBoard stops the motors after 500 ms without a valid frame
static const int EXCHANGE_PERIOD_MS = 50;
static const int HANDSHAKE_TIMEOUT_MS = 100;

bool ControlLink::start(const control_link_pins_t& pins) {
    this->pins = pins;

    spi_bus_config_t bus = {};
    bus.mosi_io_num = pins.mosi;
    bus.miso_io_num = pins.miso;
    bus.sclk_io_num = pins.sclk;
    bus.quadwp_io_num = -1;
    bus.quadhd_io_num = -1;
    bus.max_transfer_sz = FRAME_LEN;
    esp_err_t err = spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(err));
        return false;
    }

    spi_device_interface_config_t dev = {};
    dev.mode = 0;
    dev.clock_speed_hz = 1000000; // conservative for the first wiring test
    dev.spics_io_num = pins.cs;
    dev.queue_size = 1;
    err = spi_bus_add_device(SPI2_HOST, &dev, &device);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_add_device failed: %s", esp_err_to_name(err));
        return false;
    }

    ready = xSemaphoreCreateBinary();
    if (ready == nullptr) {
        ESP_LOGE(TAG, "cannot create handshake semaphore");
        return false;
    }

    gpio_config_t handshake = {};
    handshake.pin_bit_mask = 1ULL << pins.handshake;
    handshake.mode = GPIO_MODE_INPUT;
    // Keeps the line low (not ready) when no ControlBoard is plugged in
    handshake.pull_down_en = GPIO_PULLDOWN_ENABLE;
    handshake.intr_type = GPIO_INTR_POSEDGE;
    err = gpio_config(&handshake);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_config failed: %s", esp_err_to_name(err));
        return false;
    }
    err = gpio_install_isr_service(0);
    // INVALID_STATE means another module already installed the shared service
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "gpio_install_isr_service failed: %s", esp_err_to_name(err));
        return false;
    }
    err = gpio_isr_handler_add(pins.handshake, onHandshake, this);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_isr_handler_add failed: %s", esp_err_to_name(err));
        return false;
    }

    if (xTaskCreate(taskEntry, "control_link", 4096, this, 5, nullptr) != pdPASS) {
        ESP_LOGE(TAG, "cannot create task");
        return false;
    }
    return true;
}

void ControlLink::taskEntry(void* self) {
    static_cast<ControlLink*>(self)->run();
}

void IRAM_ATTR ControlLink::onHandshake(void* self) {
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(static_cast<ControlLink*>(self)->ready, &woken);
    portYIELD_FROM_ISR(woken);
}

void ControlLink::waitForControlBoard() {
    while (xSemaphoreTake(ready, pdMS_TO_TICKS(HANDSHAKE_TIMEOUT_MS)) != pdTRUE) {
        // Already high: the slave armed before our ISR existed (e.g. both boards booting)
        if (gpio_get_level(pins.handshake) == 1) {
            return;
        }
        ESP_LOGW(TAG, "ControlBoard not ready");
    }
}

void ControlLink::run() {
    uint8_t seq = 0;
    bool first = true;
    while (true) {
        waitForControlBoard();

        encodeNop(txFrame, seq);
        spi_transaction_t transaction = {};
        transaction.length = FRAME_LEN * 8;
        transaction.tx_buffer = txFrame;
        transaction.rx_buffer = rxFrame;
        esp_err_t err = spi_device_transmit(device, &transaction);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "spi_device_transmit failed: %s", esp_err_to_name(err));
        } else {
            reply_t reply;
            if (!decodeReply(rxFrame, &reply)) {
                ESP_LOGW(TAG, "bad reply (magic 0x%02x)", rxFrame[0]);
            } else {
                // Replies run one exchange behind: they echo the previous request's seq
                if (!first && reply.seq != (uint8_t)(seq - 1)) {
                    ESP_LOGW(TAG, "reply seq %u, expected %u", reply.seq, (uint8_t)(seq - 1));
                }
                ESP_LOGI(TAG, "status %u flags%s%s%s x %.1f mm y %.1f mm theta %.3f rad",
                         reply.status,
                         (reply.flags & FLAG_RESTARTED) ? " RESTARTED" : "",
                         (reply.flags & FLAG_ODOMETRY_NOT_READY) ? " ODOMETRY_NOT_READY" : "",
                         (reply.flags & FLAG_BATTERY_LOW) ? " BATTERY_LOW" : "",
                         reply.x, reply.y, reply.theta);
            }
            first = false;
            seq++;
        }

        vTaskDelay(pdMS_TO_TICKS(EXCHANGE_PERIOD_MS));
    }
}
