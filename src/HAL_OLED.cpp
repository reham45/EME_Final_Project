#include "HAL_OLED.h"
#include "HAL_VibSensor.h"
#include "System_Config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*Global Object*/
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

/*initiliaze the OLED*/
void OLED_init(void)
{
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN); 
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
    {
        sendLog("OLED failed!");
        for (;;);
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 20);
    display.println("Vibration Node");
    display.setCursor(15, 35);
    display.println("Machine is Active!");
    display.display();
}

/*OLED Display Task*/
void OLED_DISPLAY_TASK(void* Parameters)
{
    xEventGroupWaitBits(xSystemEvents, ALL_SYNC_BITS, pdFALSE, pdTRUE, portMAX_DELAY);
    VibrationData_t rxData;

    for (;;)
    {
        if (xQueueReceive(xDisplayQueue, &rxData, portMAX_DELAY) == pdPASS)
        {
            while (uxQueueMessagesWaiting(xDisplayQueue) > 0)
            {
                xQueueReceive(xDisplayQueue, &rxData, 0);
            }

            if (xSemaphoreTake(xI2cMutex, pdMS_TO_TICKS(100)) == pdTRUE)
            {
                display.clearDisplay();
                display.setTextSize(1);
                display.setTextColor(SSD1306_WHITE);

                display.setCursor(0, 0);
                display.println(F("=== Sensor INFERENCE ==="));

                display.setCursor(0, 16);
                display.printf("RMS Val : %.1f\n", rxData.rawFeatureSample);

                display.setCursor(0, 30);
                display.printf("Class: %s\n", rxData.label);

                display.setCursor(0, 44);
                display.printf("Confid. : %u%%\n", rxData.confidence);

                if (rxData.state == FAULT_STATE)
                {
                    display.setCursor(0, 56);
                    display.println(F(">> MODEL FAULT ALERT <<"));
                }

                display.display();
                xSemaphoreGive(xI2cMutex);
            }
            vTaskDelay(pdMS_TO_TICKS(DISPLAY_INTERVAL_MS));
        }
    }
    vTaskDelete(NULL);
}

/*initiliaze the alarm task and physical pins*/
void Alarm_TASK_init(void)
{
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RGB_R_PIN, OUTPUT);
    pinMode(RGB_G_PIN, OUTPUT);
    pinMode(RGB_B_PIN, OUTPUT);

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RGB_R_PIN, LOW);
    digitalWrite(RGB_G_PIN, LOW);
    digitalWrite(RGB_B_PIN, LOW);
}

/*Alarm Task logic for LED & Buzzer*/
void Alarm_Task(void* pvParameters)
{ 
    xEventGroupWaitBits(xSystemEvents, ALL_SYNC_BITS, pdFALSE, pdTRUE, portMAX_DELAY);
    VibrationData_t Classification = {0};

    for (;;)
    {
        if (xQueueReceive(xAlarmQueue, &Classification, portMAX_DELAY) == pdPASS) 
        {
            if (Classification.state == NORMAL_STATE)
            { 
                digitalWrite(RGB_R_PIN, LOW);
                digitalWrite(RGB_G_PIN, HIGH);
                digitalWrite(RGB_B_PIN, LOW);
                noTone(BUZZER_PIN);
            } 
            else if(Classification.state == ELEVATED_STATE)
            {
                digitalWrite(RGB_R_PIN, LOW);
                digitalWrite(RGB_G_PIN, LOW);
                digitalWrite(RGB_B_PIN, HIGH);
                noTone(BUZZER_PIN);
            }
            else if(Classification.state == FAULT_STATE)
            {
                digitalWrite(RGB_R_PIN, HIGH);
                digitalWrite(RGB_G_PIN, LOW);
                digitalWrite(RGB_B_PIN, LOW);
                tone(BUZZER_PIN, 2000); 
            }
            vTaskDelay(pdMS_TO_TICKS(ALARM_INTERVAL_MS));
        }
    }
    vTaskDelete(NULL);
}