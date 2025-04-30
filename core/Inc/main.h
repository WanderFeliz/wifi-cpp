#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "Gpio.h"


class Main final
{
    public:
        esp_err_t setup(void);
        void loop(void);

        Gpio::GpioOutput led{GPIO_NUM_2}; // GPIO_NUM_2 is the pin number for the LED
};