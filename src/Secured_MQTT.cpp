#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "System_Config.h"
#include "HAL_VibSensor.h" 

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 8883; 
const char* topic_telemetry = "gp_eme/mariam/vibration"; 
const char* topic_logs = "gp_eme/mariam/logs"; 

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

void sendLog(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Serial.print("[LOG] ");
    Serial.println(buffer);
    
    if (mqttClient.connected()) {
        mqttClient.publish(topic_logs, buffer);
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

void publishTelemetry(VibrationData_t data) {
    if (!mqttClient.connected()) { connectToMQTT(); }
    mqttClient.loop(); 

    JsonDocument doc;
    doc["rms"] = data.rawFeatureSample;
    doc["state"] = data.label; 
    doc["confidence"] = data.confidence;

    String payload;
    serializeJson(doc, payload); 

    mqttClient.publish(topic_telemetry, payload.c_str());
}

void vMqttTask(void *pvParameters) {
    xEventGroupWaitBits(xSystemEvents, ALL_SYNC_BITS, pdFALSE, pdTRUE, portMAX_DELAY);
    VibrationData_t rxData;

    for (;;) {
        if (xQueueReceive(xMqttQueue, &rxData, portMAX_DELAY) == pdPASS) {
            publishTelemetry(rxData);
            sendLog("Data published - RMS: %.2f, State: %s", rxData.rawFeatureSample, rxData.label);
        }
    }
}