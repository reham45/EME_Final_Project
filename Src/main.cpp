#include <Arduino.h>
#include <esp_task_wdt.h>
#include "hal_VibSensor.h"
#include "hal_Oled.h"
#include "Secured_MQTT.h"

#define STACK_SIZE 3072
#define STACK_SIZE_MQTT_TLS 6144
#define TWDT_TIMEOUT_SEC    8
void setup()
{
    Serial.begin(115200);
    delay(500);
    /*initialization*/
    Serial.println(F("\n--- Starting TinyML Industrial Vibration Node ---"));
    
    /*Allocate RTOS Primitives */
    xDisplayQueue  = xQueueCreate(3, sizeof(VibrationData_t));
    xAlarmQueue    = xQueueCreate(3, sizeof(VibrationData_t));
    xMqttQueue     = xQueueCreate(3, sizeof(VibrationData_t));
    xI2cMutex      = xSemaphoreCreateMutex();
    xSystemEvents  = xEventGroupCreate();
    
    if (!xDisplayQueue || !xAlarmQueue || !xMqttQueue || !xI2cMutex || !xSystemEvents)
    {
        Serial.println(F("[FATAL] FreeRTOS primitives allocation failed!"));
        for (;;);
    }
    /*Initialize WDT*/
    esp_task_wdt_init(TWDT_TIMEOUT_SEC, true);
    
    
    VIB_SENSOR_init(SENSOR_PIN);
    xEventGroupSetBits(xSystemEvents, BIT_SENSOR_READY);
    
    OLED_init();
    xEventGroupSetBits(xSystemEvents, BIT_OLED_READY);
    
    Alarm_TASK_init(BUZZ_PIN,RGB_G_PIN,RGB_B_PIN,RGB_R_PIN);
    xEventGroupSetBits(xSystemEvents, BIT_ALARM_READY);

    mqtt_init();
    xEventGroupSetBits(xSystemEvents, BIT_MQTT_READY);
    

    /*Task_1:Sensor_Task and check the Error */
    xTaskCreatePinnedToCore(VIB_SENSOR_READ,"Vibration sensor is Reading...",STACK_SIZE, NULL,3, NULL, 1);
    if(xTaskCreatePinnedToCore(VIB_SENSOR_READ,"Vibration sensor is Reading...",STACK_SIZE, NULL,3, NULL, 1)==pdFALSE)
    {
        Serial.println("The Sensor Error");
    }
    /*Task_2:OLED_Display  and check the Error */
    xTaskCreatePinnedToCore(OLED_DISPLAY_TASK,"OLED will DISPLAY...",STACK_SIZE, NULL,2, NULL, 0);
    if(xTaskCreatePinnedToCore(OLED_DISPLAY_TASK,"OLED will DISPLAY...",STACK_SIZE, NULL,2, NULL, 0)==pdFALSE)
    {
        Serial.println("The OLED Error");
    }
    /*Task_3:Alarm_Task  and check the Error */
    xTaskCreatePinnedToCore(Alarm_Task,"Alarm_Task...",STACK_SIZE, NULL,4, NULL, 1);
    if(xTaskCreatePinnedToCore(Alarm_Task,"Alarm_Task...",STACK_SIZE, NULL,4, NULL, 1)==pdFALSE)
    {
        Serial.println("The Alarm Task Error");
    }
    /*Task_4:Secured_MQTT_Task  and check the Error */
    xTaskCreatePinnedToCore(vMqttTask,"SecureMqttTask",STACK_SIZE_MQTT_TLS,NULL,2,NULL,0);
    if(xTaskCreatePinnedToCore(vMqttTask,"SecureMqttTask",STACK_SIZE_MQTT_TLS,NULL,2,NULL,0);==pdFALSE)
    {
        Serial.println("The Error!");
    }


    Serial.println(F("[INFO] TinyML Tasks Successfully!"));
}

void loop()
{
    vTaskDelete(NULL);
}