#include "Wifi.hpp"

namespace WIFI
{
    Wifi::state_e Wifi::_state{state_e::NOT_INITIALIZED}; // Current state of the Wi-Fi module
    char Wifi::mac_add_cstr[]{}; // MAC address string

    std::mutex Wifi::init_mutx; // lock to prevent multiple threads from accessing the constructor at the same time

    // Constructor
    // It uses a mutex to ensure that only one instance of the class is created at a time
    // Removed flag first_call and using get_mac() to check if the constructor has been called before
    Wifi::Wifi(void)
    {
        std::lock_guard<std::mutex> guard(init_mutx); // guard the mutex

        if (!get_mac()[0]) // check if the constructor has not been called before
        {
            if (ESP_OK != _get_mac())
                esp_restart(); // restart the ESP32 if MAC address could not be retrieved
        }
    }

    esp_err_t Wifi::_get_mac(void)
    {
        uint8_t mac_byte_buffer[6]{};

        const esp_err_t
            status{
                esp_efuse_mac_get_default(mac_byte_buffer)};

        if (ESP_OK == status)
        {
            snprintf(mac_add_cstr, sizeof(mac_add_cstr), "%02X:%02X:%02X:%02X:%02X:%02X",
                     mac_byte_buffer[0],
                     mac_byte_buffer[1],
                     mac_byte_buffer[2],
                     mac_byte_buffer[3],
                     mac_byte_buffer[4],
                     mac_byte_buffer[5]);

            ESP_LOGI(LOG_TAG, "MAC address: %s", mac_add_cstr);
        }
        else
        {
            ESP_LOGE(LOG_TAG, "Failed to get MAC address: %s", esp_err_to_name(status));
        }

        return status;
    }
} // namespace WIFI