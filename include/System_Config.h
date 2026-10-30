#pragma once
#define SYSTEM_CONFIG_H
#include <Arduino.h>

#define TINYML_FEATURE_COUNT 10

/*Machine States*/
typedef enum {
    NORMAL_STATE = 0,
    ELEVATED_STATE = 1,
    FAULT_STATE = 2
} MachineState_t;

// --- Data Structures ---
// This struct will be used to pass data between ML and the UI/Network
typedef struct {
  float rms_value;
  MachineState_t stateEnum; // <--- تم إضافة هذا الحقل لحل مشكلة الـ Error
  char ml_state[15]; // "NORMAL", "ELEVATED", "FAULT"
  float confidence;  // نسبة الدقة من 0.0 إلى 100.0
} SystemState_t;

// --- Function Prototypes (Interfaces) ---
// By declaring these here, main.cpp can call them even though they are written in other .cpp files.

void setupWiFi();
void setupMQTT();
void connectToMQTT();
void vMqttTask(void *pvParameters); // <--- إضافة تعريف الدالة هنا لكي تراها main.cpp
void sendLog(const char* format, ...);
SystemState_t runInference(float current_rms);

