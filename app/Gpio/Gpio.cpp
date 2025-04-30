#include "Gpio.h"

namespace Gpio
{
    [[nodiscard]] esp_err_t GpioBase::init(void)
    {
        esp_err_t status{ESP_OK};
        // Initialize the GPIO pin with the provided configuration
        // Status is set to ESP_OK and in case of error, it will be set to a non-zero value
        status |= gpio_config(&_config);

        return status;
    }

    [[nodiscard]] esp_err_t GpioOutput::init(void)
    {
        esp_err_t status{GpioBase::init()};
        // Set the initial state of the GPIO 
        if (ESP_OK == status) {
            status |= set(_inverted_logic); // if inverted_logic is true, set to 1, else set to 0
        }
        
        return status;
    }

    esp_err_t GpioOutput::set(const bool state)
    {
        _state = state; // mapp the user desired value

        return gpio_set_level(
            _pin,
            _inverted_logic ? !state : state); // set the pin level to the desired value
    }
} // namespace Gpio
