#include "wifi.h"

#include <cstring>
#include <cstdint>

#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>

#include "settings.h"

static esp_netif_t* netif;
static esp_event_handler_instance_t ip_event_handler;
static esp_event_handler_instance_t wifi_event_handler;
static EventGroupHandle_t wifi_event_group = NULL;
static wifi_config_t config;

static const char* TAG = "WIFI";
static const EventBits_t WIFI_SUCCESS = BIT0;
static const EventBits_t WIFI_FAILURE = BIT1;

void init_nvs();
void init_adapter();
void init_handlers();
void setup_wifi_ap();
void setup_wifi_client();
void setup_wifi_ip();
void wait_wifi_online();
wifi_auth_mode_t get_next_auth();

static void wifi_event_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ip_event_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

void setup_wifi(){
	init_nvs();
	init_adapter();
	init_handlers();
	if( minibot::settings::WIFI_MODE == "ap" )
		setup_wifi_ap();
	else
		setup_wifi_client();
	ESP_ERROR_CHECK(esp_wifi_start());
	wait_wifi_online();
	setup_wifi_ip();
}


void init_nvs(){
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
}


void init_adapter(){
	ESP_ERROR_CHECK( esp_netif_init() );
	ESP_ERROR_CHECK( esp_event_loop_create_default() );
	// ESP_ERROR_CHECK( esp_wifi_set_default_wifi_sta_handlers() );

	if( minibot::settings::WIFI_MODE == "ap" )
		netif = esp_netif_create_default_wifi_ap();
	else
		netif = esp_netif_create_default_wifi_sta();
	if(netif == NULL){
		ESP_LOGE(TAG, "Failed to create default WiFi STA interface");
		return;
	}

	wifi_event_group = xEventGroupCreate();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
}


void init_handlers(){
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		WIFI_EVENT,
		ESP_EVENT_ANY_ID,
		&wifi_event_callback,
		NULL,
		&wifi_event_handler));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		IP_EVENT,
		ESP_EVENT_ANY_ID,
		&ip_event_callback,
		NULL,
		&ip_event_handler));
}


void setup_wifi_client(){
	ESP_LOGI(TAG, "Configuring wireless as client station");
	strncpy((char*)config.sta.ssid,     minibot::settings::WIFI_SSID.c_str(), sizeof(config.sta.ssid));
	strncpy((char*)config.sta.password, minibot::settings::WIFI_PASS.c_str(), sizeof(config.sta.password));
	config.sta.threshold.authmode = get_next_auth();
	// config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
	// config.sta.sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER;

	// Disable powersave
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE)); // default is WIFI_PS_MIN_MODEM
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM)); // default is WIFI_STORAGE_FLASH

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
}

void setup_wifi_ap(){
	ESP_LOGI(TAG, "Configuring wireless as access point");
	config.ap.ssid_len     = minibot::settings::WIFI_SSID.length();
	config.ap.max_connection = 3;
	config.ap.pmf_cfg.required = true;
	config.ap.authmode = WIFI_AUTH_WPA2_PSK;

	strncpy((char*)config.ap.ssid,     minibot::settings::WIFI_SSID.c_str(), sizeof(config.sta.ssid));
	strncpy((char*)config.ap.password, minibot::settings::WIFI_PASS.c_str(), sizeof(config.sta.password));

	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE)); // default is WIFI_PS_MIN_MODEM
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM)); // default is WIFI_STORAGE_FLASH

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &config));
}


void wait_wifi_online(){
	// EventBits_t bits =
	xEventGroupWaitBits(wifi_event_group,
	WIFI_SUCCESS,	// WIFI_SUCCESS | WIFI_FAILURE,
	pdFALSE, pdFALSE, portMAX_DELAY);
}


void setup_wifi_ip(){

}


wifi_auth_mode_t get_next_auth(){
	static uint8_t ix = 0;
	static wifi_auth_mode_t auths[] = {
		WIFI_AUTH_WPA_WPA2_PSK,
		WIFI_AUTH_WPA2_WPA3_PSK,
		WIFI_AUTH_WEP,
		WIFI_AUTH_WAPI_PSK,
		WIFI_AUTH_WPA_PSK,
		WIFI_AUTH_WPA2_PSK,
		WIFI_AUTH_WPA3_PSK,
	};
	if( minibot::settings::WIFI_PASS.empty() )
		return WIFI_AUTH_OPEN;
	wifi_auth_mode_t auth = auths[ix];
	ix = (ix+1) % 7;
	return auth;
}

void disconnect(void){
	if (wifi_event_group)
		vEventGroupDelete(wifi_event_group);
	esp_wifi_disconnect();
	esp_wifi_deinit();
}


static
void ip_event_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
	ESP_LOGI(TAG, "Handling IP event, event code 0x%" PRIx32, event_id);
	if (event_base != IP_EVENT) return;
    if (event_id != IP_EVENT_STA_GOT_IP) return;

	ip_event_got_ip_t *event_ip = (ip_event_got_ip_t *)event_data;
	ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event_ip->ip_info.ip));
	if( minibot::settings::WIFI_ADDR.empty() )
		xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
}


static
void wifi_event_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
	ESP_LOGI(TAG, "Handling Wi-Fi event, event code 0x%" PRIx32, event_id);
	if (event_base != WIFI_EVENT) return;

	switch (event_id){
		case WIFI_EVENT_STA_START:
			ESP_LOGI(TAG, "Wi-Fi started, connecting to %s", minibot::settings::WIFI_SSID.c_str());
			esp_wifi_connect();
			break;

		case WIFI_EVENT_STA_STOP:
			ESP_LOGI(TAG, "Wi-Fi stopped");
			break;

		case WIFI_EVENT_STA_CONNECTED:
			ESP_LOGI(TAG, "Wi-Fi connected 0x%" PRIx32, WIFI_EVENT_STA_CONNECTED);
			if( !minibot::settings::WIFI_ADDR.empty() )
				xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
			break;

		case WIFI_EVENT_STA_DISCONNECTED: // Or timeout
			ESP_LOGI(TAG, "Disconnected. Reconnecting to connecting to %s", minibot::settings::WIFI_SSID.c_str());
			// config.sta.threshold.authmode = get_next_auth();
			esp_wifi_connect();
			break;
	}
}

