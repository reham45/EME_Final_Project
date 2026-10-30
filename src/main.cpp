#include <Arduino.h>
#include "System_Config.h" 
#include "HAL_VibSensor.h"
#include "HAL_OLED.h"


#define STACK_SIZE 2048
#define STACK_SIZE_MQTT_TLS 6144
#define TWDT_TIMEOUT_SEC 16

// Queues & Global Var
QueueHandle_t xDisplayQueue;
QueueHandle_t xAlarmQueue;
QueueHandle_t xMqttQueue;
SemaphoreHandle_t xI2cMutex;
EventGroupHandle_t xSystemEvents;

// Network & ML Functions
extern void setupWiFi();
extern void setupMQTT();
extern void connectToMQTT();

void setup()
{
    Serial.begin(115200);
    delay(500);
    sendLog("\n--- Starting TinyML Industrial Vibration Node ---");
    
    setupWiFi();
    setupMQTT();
    connectToMQTT(); 

    /*Allocate RTOS Primitives */
    xDisplayQueue  = xQueueCreate(3, sizeof(VibrationData_t));
    xAlarmQueue    = xQueueCreate(3, sizeof(VibrationData_t));
    xMqttQueue     = xQueueCreate(3, sizeof(VibrationData_t));
    xI2cMutex      = xSemaphoreCreateMutex();
    xSystemEvents  = xEventGroupCreate();
    
    if (!xDisplayQueue || !xAlarmQueue || !xMqttQueue || !xI2cMutex || !xSystemEvents)
    {
        sendLog("[FATAL] FreeRTOS primitives allocation failed!");
        for (;;);
    }

    /*esp_task_wdt_init(TWDT_TIMEOUT_SEC, true);  */   
    VIB_SENSOR_init();
    xEventGroupSetBits(xSystemEvents, BIT_SENSOR_READY);
    
    OLED_init();
    xEventGroupSetBits(xSystemEvents, BIT_OLED_READY);
    
    Alarm_TASK_init();
    xEventGroupSetBits(xSystemEvents, BIT_ALARM_READY);

    xEventGroupSetBits(xSystemEvents, BIT_MQTT_READY);
    
    /*Tasks Creation*/
    xTaskCreatePinnedToCore(VIB_SENSOR_READ,"Vib_Sensor",STACK_SIZE, NULL,3, NULL, 1);
    xTaskCreatePinnedToCore(OLED_DISPLAY_TASK,"OLED_Display",STACK_SIZE, NULL,2, NULL, 0);
    xTaskCreatePinnedToCore(Alarm_Task,"Alarm_Task",STACK_SIZE, NULL,4, NULL, 1);
    

    xTaskCreatePinnedToCore(vMqttTask,"SecureMqttTask",STACK_SIZE_MQTT_TLS,NULL,2,NULL,0);

    sendLog("[INFO] System Boot Complete. Tasks starting...");
}

void loop()
{
    vTaskDelete(NULL);
}