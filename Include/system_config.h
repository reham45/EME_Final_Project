#pragma once
#include <Arduino.h>

// --- Data Structures ---
// This struct will be used to pass data between ML and the UI/Network
typedef struct {
  float rms_value;
  char ml_state[15]; // "NORMAL", "ELEVATED", "FAULT"
  float confidence;  // نسبة الدقة من 0.0 إلى 100.0
} SystemState_t;

// --- Function Prototypes (Interfaces) ---
// By declaring these here, main.cpp can call them even though they are written in other .cpp files.

// From Wifi_Manager.cpp
void setupWiFi();

// From MQTT.cpp
void setupMQTT();
void publishTelemetry(SystemState_t data);
void sendLog(String message);

// From ML_Interface.cpp
SystemState_t runInference(float current_rms);
