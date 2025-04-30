#pragma once

#include <esp_wifi.h>

namespace WIFI
{
    class WIFI
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

            esp_err_t init(void); // Initialize the Wi-Fi module + config
            esp_err_t begin(void); // Start the Wi-Fi module, connect to the network
            esp_err_t stop(void); // Stop the Wi-Fi module

            state_e get_state(void); // Get the current state of the Wi-Fi module
            const char* get_mac(void); // Get the MAC address of the Wi-Fi module

        private:
            void state_machine(void); // State machine for the Wi-Fi module
            
    };
} // namespace WIFI
