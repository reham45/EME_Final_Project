#include "system_config.h"
#include <WiFiManager.h>

#define BOOT_BUTTON 0 // زر الـ BOOT المدمج في الـ ESP32

void setupWiFi() {
    Serial.println("Initializing WiFi Manager...");
    
    // تهيئة زر الـ BOOT
    pinMode(BOOT_BUTTON, INPUT_PULLUP);
    
    WiFiManager wm;
    
    // لو كنتِ ضاغطة على زر BOOT أثناء عمل Restart للبوردة، هيمسح الباسورد القديم
    if (digitalRead(BOOT_BUTTON) == LOW) {
        Serial.println("BOOT button pressed! Resetting WiFi credentials...");
        wm.resetSettings();
        Serial.println("Credentials deleted. AP Mode started.");
    }

    // محاولة الاتصال التلقائي. لو فشل أو لو مسحنا الباسورد، هيفتح الـ Captive Portal
    if(!wm.autoConnect("ESP32_Industrial_Node")) {
        Serial.println("Failed to connect to WiFi");
        ESP.restart();
    } else {
        Serial.println("WiFi Connected Successfully!");
    }
}