#include "System_Config.h"
#include <WiFiManager.h>

#define BOOT_BUTTON 0 // GPIO0 is typically used for boot button on ESP32 boards

void setupWiFi() {
    Serial.println("Initializing WiFi Manager...");
    
    pinMode(BOOT_BUTTON, INPUT_PULLUP);
    
    WiFiManager wm;
    
    if (digitalRead(BOOT_BUTTON) == LOW) {
        Serial.println("BOOT button pressed! Resetting WiFi credentials...");
        wm.resetSettings();
        Serial.println("Credentials deleted. AP Mode started.");
    }

    if(!wm.autoConnect("ESP32_Industrial_Node")) {
        Serial.println("Failed to connect to WiFi");
        ESP.restart();
    } else {
        Serial.println("WiFi Connected Successfully!");
    }
}