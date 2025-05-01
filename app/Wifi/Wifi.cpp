#include "Wifi.hpp"

namespace WIFI
{
    // Wifi Statics
    Wifi::state_e Wifi::_state{state_e::NOT_INITIALIZED}; // Current state of the Wi-Fi module
    char Wifi::mac_add_cstr[]{};                          // MAC address string
    wifi_init_config_t Wifi::wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t Wifi::wifi_config{};

    // Mutex lock for initialization
    std::mutex Wifi::init_mutx{};

    // Mutex lock for connections
    std::mutex Wifi::connect_mutx{};

    // Mutex lock for state
    std::mutex Wifi::state_mutx{};

    // Constructor
    // It uses a mutex to ensure that only one instance of the class is created at a time
    // Removed flag first_call and using get_mac() to check if the constructor has been called before
    Wifi::Wifi(void)
    {
        std::lock_guard<std::mutex> init_guard(init_mutx); // guard the mutex

        if (!get_mac()[0]) // check if the constructor has not been called before
        {
            if (ESP_OK != _get_mac())
                esp_restart(); // restart the ESP32 if MAC address could not be retrieved
        }
    }

    void Wifi::event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        if (WIFI_EVENT == event_base)
        {
            ESP_LOGI(_log_tag, "%s:%d Got a WIFI_EVENT", __func__, __LINE__);
            return wifi_event_handler(arg, event_base, event_id, event_data);
        }
        else if (IP_EVENT == event_base)
        {
            ESP_LOGI(_log_tag, "%s:%d Got a IP_EVENT", __func__, __LINE__);
            return ip_event_handler(arg, event_base, event_id, event_data);
        }
        else
        {
            ESP_LOGW(_log_tag, "Unexpected event: %s", event_base); // TODO: Logging
        }
    }

    void Wifi::wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        if (WIFI_EVENT != event_base)
            return;

        const wifi_event_t event_type{static_cast<wifi_event_t>(event_id)};

        ESP_LOGI(_log_tag, "%s:%d Event ID %ld", __func__, __LINE__, event_id);

        switch (event_type)
        {
        case WIFI_EVENT_STA_START:
        {
            ESP_LOGI(_log_tag, "%s:%d STA_START, Waiting for state_mutx", __func__, __LINE__);
            std::lock_guard<std::mutex> state_guard(state_mutx);
            _state = state_e::READY_TO_CONNECT;
            ESP_LOGI(_log_tag, "%s:%d READY_TO_CONNECT", __func__, __LINE__);
            break;
        }

        case WIFI_EVENT_STA_CONNECTED:
        {
            ESP_LOGI(_log_tag, "%s:%d STA_CONNECTED, Waiting for state_mutx", __func__, __LINE__);
            std::lock_guard<std::mutex> state_guard(state_mutx);
            _state = state_e::WAITING_FOR_IP;
            ESP_LOGI(_log_tag, "%s:%d WAITING_FOR_IP", __func__, __LINE__);
            break;
        }

        default:
            // TODO: Stop and disconnected, others events
            ESP_LOGW(_log_tag, "%s:%d Default WIFI switch case (%ld)", __func__, __LINE__, event_id);
            break;
        }
    }

    void Wifi::ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        if (IP_EVENT != event_base)
            return;

        const ip_event_t event_type{static_cast<ip_event_t>(event_id)};

        ESP_LOGI(_log_tag, "%s:%d Event ID %ld", __func__, __LINE__, event_id);

        switch (event_type)
        {
        case IP_EVENT_STA_GOT_IP:
        {
            ESP_LOGI(_log_tag, "%s:%d Got a IP, Waiting for state_mutx", __func__, __LINE__);
            std::lock_guard<std::mutex> state_guard(state_mutx);
            _state = state_e::CONNECTED;
            // ESP_LOGI(_log_tag, "Got IP address: %s", ((ip_event_got_ip_t *)event_data)->ip_info.ip.addr);
            ESP_LOGI(_log_tag, "%s:%d CONNECTED!!", __func__, __LINE__);
            break;
        }
        case IP_EVENT_STA_LOST_IP:
        {
            ESP_LOGI(_log_tag, "%s:%d Lost a IP, Waiting for state_mutx", __func__, __LINE__);
            std::lock_guard<std::mutex> state_guard(state_mutx);
            _state = state_e::WAITING_FOR_IP;
            ESP_LOGI(_log_tag, "%s:%d WAITING_FOR_IP", __func__, __LINE__);
            break;
        }

        default:
            // TODO: IP v6
            ESP_LOGW(_log_tag, "%s:%d Default IP switch case (%ld)", __func__, __LINE__, event_id);
            break;
        }
    }

    esp_err_t Wifi::init(void)
    {

        return _init();
    }

    esp_err_t Wifi::begin(void)
    {
        ESP_LOGI(_log_tag, "%s:%d Waiting for connect_mutx", __func__, __LINE__);
        std::lock_guard<std::mutex> connect_guard(connect_mutx);
        esp_err_t status{ESP_OK};

        ESP_LOGI(_log_tag, "%s:%d Waiting for state_mutx", __func__, __LINE__);
        std::lock_guard<std::mutex> state_guard(state_mutx);

        switch (_state)
        {
        case state_e::READY_TO_CONNECT:
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_connect", __func__, __LINE__);
            status = esp_wifi_connect();
            ESP_LOGI(_log_tag, "%s:%d esp_wifi_connect: %s", __func__, __LINE__,
                 esp_err_to_name(status));

            if (ESP_OK == status)
                _state = state_e::CONNECTING;

            break;
        case state_e::CONNECTING:
        case state_e::WAITING_FOR_IP:
        case state_e::CONNECTED:
            break;
        case state_e::NOT_INITIALIZED:
        case state_e::INITIALIZED:
        case state_e::DISCONNECTED:
        case state_e::ERROR:
            ESP_LOGI(_log_tag, "%s:%d Error state", __func__, __LINE__);
            status = ESP_FAIL;
            break;
        }

        return status;
    }

    // Private Initialize method Wi-Fi module
    esp_err_t Wifi::_init(void)
    {
        ESP_LOGI(_log_tag, "%s:%d Waiting for init_mutx", __func__, __LINE__);
        std::lock_guard<std::mutex> init_guard(init_mutx); // guard the mutex or initialization

        esp_err_t status{ESP_OK};

        ESP_LOGI(_log_tag, "%s:%d Waiting for state_mutx", __func__, __LINE__);
        std::lock_guard<std::mutex> state_guard(state_mutx); // guard state

        if (state_e::NOT_INITIALIZED == _state)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_netif_init", __func__, __LINE__);
            status = esp_netif_init();
            ESP_LOGI(_log_tag, "%s:%d esp_netif_init: %s", __func__, __LINE__,
                esp_err_to_name(status));
            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d Calling esp_netif_create_default_wifi_sta", __func__, __LINE__);
                const esp_netif_t *const p_netif = esp_netif_create_default_wifi_sta();
                ESP_LOGI(_log_tag, "%s:%d esp_netif_create_default_wifi_sta: %p", __func__, __LINE__,
                    p_netif);

                if (!p_netif)
                    status = ESP_FAIL;
            }

            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d Calling wifi_init_config", __func__, __LINE__);
                status = esp_wifi_init(&wifi_init_config);
                ESP_LOGI(_log_tag, "%s:%d wifi_init_config: %s", __func__, __LINE__,
                    esp_err_to_name(status));
            }

            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d Calling esp_event_handler_instance_register", __func__, __LINE__);
                status = esp_event_handler_instance_register(
                    WIFI_EVENT,
                    ESP_EVENT_ANY_ID,
                    &event_handler,
                    nullptr,
                    nullptr);
                ESP_LOGI(_log_tag, "%s:%d esp_event_handler_instance_register: %s", __func__, __LINE__,
                    esp_err_to_name(status));
            }
            
            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d Calling esp_event_handler_instance_register", __func__, __LINE__);
                status = esp_event_handler_instance_register(
                    IP_EVENT,
                    ESP_EVENT_ANY_ID,
                    &event_handler,
                    nullptr,
                    nullptr);
                ESP_LOGI(_log_tag, "%s:%d esp_event_handler_instance_register: %s", __func__, __LINE__,
                    esp_err_to_name(status));
            }

            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_mode", __func__, __LINE__);
                status = esp_wifi_set_mode(WIFI_MODE_STA);
                ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_mode: %s", __func__, __LINE__,
                    esp_err_to_name(status));
            }

            if (ESP_OK == status)
            {
                // SSID and password have to be copied as they are buffers instead of pointers
                const size_t ssid_len_to_copy = std::min<size_t>(strlen(ssid), sizeof(wifi_config.sta.ssid));
                const size_t password_len_to_copy = std::min<size_t>(strlen(password), sizeof(wifi_config.sta.password));

                if (strlen(ssid) > sizeof(wifi_config.sta.ssid))
                {
                    ESP_LOGE(_log_tag, "SSID to long Max 30 caracters");
                    return ESP_FAIL;
                }
                else if (strlen(password) > sizeof(wifi_config.sta.password))
                {
                    ESP_LOGE(_log_tag, "Password to long Max 62 caracters");
                };

                memcpy(wifi_config.sta.ssid, ssid,
                       ssid_len_to_copy);
                memcpy(wifi_config.sta.password, password, password_len_to_copy);

                wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
                wifi_config.sta.pmf_cfg.required = false;

                ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_config", __func__, __LINE__);
                status = esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // TODO: Keep track of mode
                ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_config: %s", __func__, __LINE__,
                    esp_err_to_name(status));
            }

            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_start", __func__, __LINE__);
                status = esp_wifi_start();
                ESP_LOGI(_log_tag, "%s:%d esp_wifi_start: %s", __func__, __LINE__,
                    esp_err_to_name(status));
            }

            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d INITIALIZED", __func__, __LINE__);
                _state = state_e::INITIALIZED;
            }
        }
        else if (state_e::ERROR == _state)
        {
            ESP_LOGE(_log_tag, "%s:%d FAILED", __func__, __LINE__);
            _state = state_e::NOT_INITIALIZED;
        }

        return status;
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

            ESP_LOGI(_log_tag, "MAC address: %s", mac_add_cstr);
        }
        else
        {
            ESP_LOGE(_log_tag, "Failed to get MAC address: %s", esp_err_to_name(status));
        }

        return status;
    }
} // namespace WIFI