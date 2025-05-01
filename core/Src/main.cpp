#include "main.hpp"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#include <esp_log.h>

#define LOG_TAG "MAIN"

static Main app;

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Create the default event loop

    ESP_ERROR_CHECK(nvs_flash_init()); // Initialize the NVS flash storage
    
    ESP_ERROR_CHECK(app.setup());

    while (true) {
        app.loop();
    }
}

esp_err_t Main::setup(void)
{
    ESP_LOGI(LOG_TAG, "Setting up application...");
    // Initialize peripherals, configure GPIOs, etc.

    esp_err_t status{ESP_OK};

    status |= led.init(); // Initialize the LED GPIO
    status |= wifi.init(); // Initialize the Wi-Fi module

    if (ESP_OK == status) status |= wifi.begin(); // Start the Wi-Fi module

    return status;
}

void Main::loop(void)
{
    // Main loop code goes here
    // This function will be called repeatedly in the app_main loop
    ESP_LOGI(LOG_TAG, "Running main loop...");
    // Toggle the LED state every second
    led.set(true);
    vTaskDelay(pdMS_TO_TICKS(1000));
    led.set(false);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

