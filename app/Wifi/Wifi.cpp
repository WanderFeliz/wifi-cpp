#include "Wifi.hpp"

namespace WIFI
{
    char Wifi::mac_add_cstr[]{}; // MAC address string

    std::atomic_bool Wifi::first_call{false}; // Flag to check if the constructor has been called before

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