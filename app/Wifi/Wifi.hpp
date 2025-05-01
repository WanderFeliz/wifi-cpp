#pragma once

#include <algorithm>
#include <mutex>

#include <cstring>

#include <esp_wifi.h>
#include <esp_mac.h>
#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#include <esp_log.h>

namespace WIFI
{
    class Wifi
    {
        constexpr static const char* _log_tag{"WIFI"};
        constexpr static const char* ssid{"my_wifi"};
        constexpr static const char* password{"my_password"};

    public:
        // Strongly typed enum for the state of the Wi-Fi module
        // This enum is used to represent the different states of the Wi-Fi module
        enum class state_e
        {
            NOT_INITIALIZED,
            INITIALIZED,
            READY_TO_CONNECT,
            CONNECTING,
            WAITING_FOR_IP,
            CONNECTED,
            DISCONNECTED,
            ERROR,
        };

        // Rule of five for the Wifi class
        /// Default constructor method
        Wifi(void);               // Default constructor
        ~Wifi() = default;                // Destructor
        Wifi(const Wifi &) = default;            // Copy constructor
        Wifi(Wifi &&) = default;             // Move constructor
        Wifi &operator=(const Wifi &) = default; // Copy assignment operator
        Wifi &operator=(Wifi &&) = default;  // Move assignment operator

        /// @brief Initialization method for the Wifi instance
        /// @param  None
        /// @return esp_err base status depending on what happened in the initialization.
        esp_err_t init(void);
        esp_err_t begin(void);
        esp_err_t stop(void);  // TODO Stop the Wi-Fi module

        constexpr const state_e& get_state(void) {return _state;}; // This is a const referrence just a constexpr wont work as the variable is created in runtime

        constexpr static const char *get_mac(void)
        {
            return mac_add_cstr;
        }; // Get the MAC address of the Wi-Fi module

    private:

        static wifi_init_config_t wifi_init_config;
        static wifi_config_t wifi_config;
        static esp_err_t _init(void); // Initialize the Wi-Fi module

        void state_machine(void);     // State machine for the Wi-Fi module

        /**
         * Wifi base event handler
         */
        static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        
        /**
         * Conection specific event handler
         */
        static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        
        /**
         * IP event handler
         */
        static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        
        /// @brief // Current state of the Wi-Fi module
        static state_e _state;

        /// @brief Private method to get the MAC address of the Wi-Fi module
        /// Get the MAC address of the Wi-Fi module
        esp_err_t _get_mac(void);     
        
        /// @brief MAC address string
        static char mac_add_cstr[18];
        
        /// @brief lock to prevent multiple threads from accessing the constructor at the same time
        static std::mutex init_mutx;

        /**
         * lock to prevent multiple threads form accessing the connecting of the wifi at the same time 
         * 
         * */ 
        static std::mutex connect_mutx;
        
        /**
         * lock to prevent multiple threads form accessing the state of the wifi at the same time 
         * 
         * */ 
        static std::mutex state_mutx;

    };
} // namespace WIFI
