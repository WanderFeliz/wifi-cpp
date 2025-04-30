#pragma once

#include "driver/gpio.h"

namespace Gpio
{
    class GpioBase
    {
        protected:
            const gpio_num_t _pin;
            const gpio_config_t _config;
            const bool _inverted_logic = false;


        public:

            constexpr GpioBase(const gpio_num_t pin, const gpio_config_t& config, const bool inverted_logic = false):
            _pin{pin},
            _config{config},
            _inverted_logic{inverted_logic} {};

            virtual bool state(void) = 0;
            virtual esp_err_t set(const bool state) = 0;

            [[nodiscard]] esp_err_t init(void);


    }; // Class GpioBase

    class GpioOutput : public GpioBase
    {
        bool _state = false;

        public:

            constexpr GpioOutput(
                const gpio_num_t pin,
                const bool _inverted_logic = false
            ) :
            // Initialize the base class with the pin number and configuration
                GpioBase{pin,
                    gpio_config_t{
                        .pin_bit_mask   = static_cast<uint64_t>(1) << pin,
                        .mode           = GPIO_MODE_OUTPUT,
                        .pull_up_en     = GPIO_PULLUP_DISABLE,
                        .pull_down_en   = GPIO_PULLDOWN_ENABLE,
                        .intr_type      = GPIO_INTR_DISABLE,
                    }, _inverted_logic}
                {
                    // Constructor body can be empty, as initialization is done in the base class
                };
            
        

            [[nodiscard]] esp_err_t init(void);

            esp_err_t set(const bool state);
            esp_err_t toggle(void);
            bool state(void) { return _state; };
    }; // Class GpioOutput

    class GpioInput
    {
        const gpio_num_t _pin;
        const bool _inverted_logic = false;

        public:

            GpioInput(const gpio_num_t pin, const bool inverted_logic = false):
            _pin{pin}, 
            _inverted_logic{inverted_logic} {};

            esp_err_t init(void);
            bool state(void);
    }; // Class GpioInput
    
} // namespace Gpio
