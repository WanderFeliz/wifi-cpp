#pragma once

#include <mutex>

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
        // Strongly typed enum for the state of the Wi-Fi module
        // This enum is used to represent the different states of the Wi-Fi module
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

        // Rule of five for the Wifi class
        Wifi(void);               // Default constructor
        ~Wifi() = default;                // Destructor
        Wifi(const Wifi &) = default;            // Copy constructor
        Wifi(Wifi &&) = default;             // Move constructor
        Wifi &operator=(const Wifi &) = default; // Copy assignment operator
        Wifi &operator=(Wifi &&) = default;  // Move assignment operator

        esp_err_t init(void);  // TODO Initialize the Wi-Fi module + config
        esp_err_t begin(void); // TODO Start the Wi-Fi module, connect to the network
        esp_err_t stop(void);  // TODO Stop the Wi-Fi module

        state_e get_state(void); // Get the current state of the Wi-Fi module

        constexpr static const char *get_mac(void)
        {
            return mac_add_cstr;
        }; // Get the MAC address of the Wi-Fi module

    private:

        static esp_err_t _init(void); // Initialize the Wi-Fi module

        void state_machine(void);     // State machine for the Wi-Fi module
        static state_e _state; // Current state of the Wi-Fi module

        // Private method to get the MAC address of the Wi-Fi module
        esp_err_t _get_mac(void);     // Get the MAC address of the Wi-Fi module
        
        static char mac_add_cstr[18]; // MAC address string
        static std::mutex init_mutx; // lock to prevent multiple threads from accessing the constructor at the same time

    };
} // namespace WIFI
