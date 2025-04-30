#pragma once

#include <atomic>

#include <esp_wifi.h>
#include <esp_mac.h>
#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#include <esp_log.h>

#define LOG_TAG "WIFI"

namespace WIFI
{
    class Wifi
    {
    public:
        enum class state_e
        {
            NOT_INITIALIZED,
            INITIALIZED,
            WAITING_FOR_CREDENTIALS,
            READY_TO_CONNECT,
            CONNECTING,
            CONNECTED,
            DISCONNECTED,
            ERROR,
        };

        Wifi(void)
        {
            if (!first_call) // Check if the constructor has been called before
            {
                if (ESP_OK != _get_mac()) esp_restart(); // Get the MAC address of the Wi-Fi module
                first_call = true; // Set the flag to true to indicate that the constructor has been called
            } // Constructor
            else
            {
                ESP_LOGI(LOG_TAG, "Wifi constructor already called, skipping MAC address retrieval.");
            } // Constructor
              
        } // Constructor


        esp_err_t init(void);  // Initialize the Wi-Fi module + config
        esp_err_t begin(void); // Start the Wi-Fi module, connect to the network
        esp_err_t stop(void);  // Stop the Wi-Fi module

        state_e get_state(void); // Get the current state of the Wi-Fi module

        const char *get_mac(void)
        {
            return mac_add_cstr;
        }; // Get the MAC address of the Wi-Fi module

    private:
        void state_machine(void);     // State machine for the Wi-Fi module
        esp_err_t _get_mac(void);     // Get the MAC address of the Wi-Fi module
        static char mac_add_cstr[13]; // MAC address string

        static std::atomic_bool first_call; // Flag to check if the constructor has been called before
    };
} // namespace WIFI
