//
// Created by  changjin on 2021/02/25.
//

#ifndef OTA_ESP32_OTA_H
#define OTA_ESP32_OTA_H

#include <WiFi.h>
#include <ESPmDNS.h>
#include "esp_wps.h"

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "low house IOT"
#define ESP_DEVICE_NAME   "low house Workshop Controller"

static esp_wps_config_t config;

char g_nameprefix[100];

void wpsInitConfig(){
    config.crypto_funcs = &g_wifi_default_wps_crypto_funcs;
    config.wps_type = ESP_WPS_MODE;
    strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
    strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
    strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
    strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}

String wpspin2string(uint8_t a[]){
    char wps_pin[9];
    for(int i=0;i<8;i++){
        wps_pin[i] = a[i];
    }
    wps_pin[8] = '\0';
    return (String)wps_pin;
}

void ota_handle( void * parameter ) {
    for (;;) {
        ArduinoOTA.handle();
        delay(3500);
    }
}


void setupOTA() {
    // Configure the hostname
    uint16_t maxlen = strlen(g_nameprefix) + 7;
    char *fullhostname = new char[maxlen];
    uint8_t mac[6];
    WiFi.macAddress(mac);
    snprintf(fullhostname, maxlen, "%s-%02x%02x%02x", g_nameprefix, mac[3], mac[4], mac[5]);
    ArduinoOTA.setHostname(fullhostname);
    delete[] fullhostname;

    // Configure and start the WiFi station
//    WiFi.mode(WIFI_STA);
//    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232); // Use 8266 port if you are working in Sloeber IDE, it is fixed there and not adjustable

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        //NOTE: make .detach() here for all functions called by Ticker.h library - not to interrupt transfer process in any way.
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("\nAuth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("\nBegin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("\nConnect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("\nReceive Failed");
        else if (error == OTA_END_ERROR) Serial.println("\nEnd Failed");
    });

    ArduinoOTA.begin();

    Serial.println("OTA Initialized");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    xTaskCreate(
        ota_handle,          /* Task function. */
        "OTA_HANDLE",        /* String with name of task. */
        10000,            /* Stack size in bytes. */
        NULL,             /* Parameter passed as input of the task */
        1,                /* Priority of the task. */
        NULL
    );            /* Task handle. */
}

void WiFiEvent(WiFiEvent_t event, system_event_info_t info){
    switch(event){
        case SYSTEM_EVENT_STA_START:
            Serial.println("Station Mode Started");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.println("Connected to :" + String(WiFi.SSID()));
            Serial.print("Got IP: ");
            Serial.println(WiFi.localIP());

            setupOTA();
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from station, attempting reconnection");
            WiFi.reconnect();

            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()));
            esp_wifi_wps_disable();
            delay(10);
            WiFi.begin();
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("WPS Failed, retrying");
            esp_wifi_wps_disable();
            esp_wifi_wps_enable(&config);
            esp_wifi_wps_start(0);
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("WPS Timedout, retrying");
//            esp_wifi_wps_disable();
//            esp_wifi_wps_enable(&config);
//            esp_wifi_wps_start(0);
            ESP.restart();
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("WPS_PIN = " + wpspin2string(info.sta_er_pin.pin_code));
            break;
        default:
            break;
    }
}


void setupWPS_OTA(const char* nameprefix) {
    size_t len  = strlen(nameprefix);
    for (int i = 0; i < len; ++i) {
        g_nameprefix[i] = nameprefix[i];
        g_nameprefix[i+1] = 0;
    }

    wl_status_t state;

    WiFi.onEvent(WiFiEvent);
    // it reconnect with last success connection info.
    WiFi.begin();
    delay(3000);
    state = WiFi.status();
    Serial.println(state);

    if(state != WL_CONNECTED) {
        Serial.println("not connected yet!");

        WiFi.mode(WIFI_MODE_STA);
        Serial.println("Starting WPS");

        wpsInitConfig();
        esp_wifi_wps_enable(&config);
        esp_wifi_wps_start(0);
    }else {
        Serial.println(" connected !");
    }
}


#endif //OTA_ESP32_OTA_H
