#include "system_config.h"
#include "hal_VibSensor.h" 
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 8883; 
const char* topic_telemetry = "gp_eme/mariam/vibration"; 
const char* topic_logs = "gp_eme/mariam/logs"; 

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// ==========================================
// Print to Serial Monitor & Dashboard Logs
// ==========================================
void sendLog(String message) {
    // 1.Serial Print
    Serial.println("[LOG] " + message);
    
    // 2.Seind to dashboard if connected
    if (mqttClient.connected()) {
        mqttClient.publish(topic_logs, message.c_str());
    }
}

void setupMQTT() {
    sendLog("Setting up MQTT via TLS...");
    espClient.setInsecure(); 
    mqttClient.setServer(mqtt_server, mqtt_port);
}

void connectToMQTT() {
    while (!mqttClient.connected()) {
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        if (mqttClient.connect(clientId.c_str())) {
            sendLog("Connected to HiveMQ successfully!");
        } else {
            vTaskDelay(pdMS_TO_TICKS(5000)); 
        }
    }
}

// ==========================================
// Struct to JSON
// ==========================================

void publishTelemetry(VibrationData_t data) {
    if (!mqttClient.connected()) { connectToMQTT(); }
    mqttClient.loop(); 

    StaticJsonDocument<200> doc;
    doc["rms"] = data.rawFeatureSample;
    doc["state"] = data.label; // "normal", "elevated", or "fault"
    doc["confidence"] = data.confidence;

    String payload;
    serializeJson(doc, payload); 

    mqttClient.publish(topic_telemetry, payload.c_str());
}

// ==========================================
// Task MQTT
// ==========================================
void vMqttTask(void *pvParameters) {

    xEventGroupWaitBits(xSystemEvents, ALL_SYNC_BITS, pdFALSE, pdTRUE, portMAX_DELAY);
    
    VibrationData_t rxData;

    for (;;) {
        
        if (xQueueReceive(xMqttQueue, &rxData, portMAX_DELAY) == pdPASS) {
            
            // Send Data HiveMQ
            publishTelemetry(rxData);
            
            sendLog("Data published - RMS: " + String(rxData.rawFeatureSample) + ", State: " + String(rxData.label));
        }
    }
}
