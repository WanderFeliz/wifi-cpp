#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include "Gpio.hpp"
#include "Wifi.hpp"


class Main final
{
    public:
        esp_err_t setup(void);
        void loop(void);

        Gpio::GpioOutput led{GPIO_NUM_2}; // GPIO_NUM_2 is the pin number for the LED
        WIFI::Wifi wifi{}; // Create an instance of the Wifi class
};