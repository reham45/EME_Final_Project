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
  MachineState_t stateEnum;
  char ml_state[15];
  float confidence; 
} SystemState_t;

// --- Function Prototypes (Interfaces) ---
// By declaring these here, main.cpp can call them even though they are written in other .cpp files.

void setupWiFi();
void setupMQTT();
void connectToMQTT();
void vMqttTask(void *pvParameters); 
void sendLog(const char* format, ...);
SystemState_t runInference(float current_rms);

