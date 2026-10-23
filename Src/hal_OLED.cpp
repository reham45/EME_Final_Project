#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "hal_Oled.h"


/*initiliaze the OLED*/
void OLED_init(void)
{
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
}

void OLED_TASK(void* Parameters)
{

    vTaskDelay(pdMS_TO_TICKS(DISPLAY_INTERVAL_MS));


    
    xTaskDelete(NULL);
}



/*initiliaze the alarm task*/
void Alarm_TASK_init(gpio_num_t BUZZER_PIN,gpio_num_t NORMALSTATE,gpio_num_t ELEVATEDSTATE,gpio_num_t ALARMSTATE)
{
    pinMode(BUZZER_PIN,OUTPUT);
    pinMode(NORMALSTATE,OUTPUT);
    pinMode(ELEVATEDSTATE,OUTPUT);
    pinMode(FAULTSTATE,OUTPUT);
}
/*Detect the classification */
void Alarm_Task(void*pvParameters)
{ 
    str Classification="";

    for (;;)
    {
        if (xQueueReceive(xAlarmQueue, &alarmState, portMAX_DELAY) == pdPASS) 
        {
            if (Classification == "normal")
            {
                digitalWrite(BUZZER_PIN, LOW);
                digitalWrite(RGB_R_PIN, LOW);
                digitalWrite(RGB_G_PIN, HIGH);
                digitalWrite(RGB_B_PIN, LOW);
                vTaskDelay(pdMS_TO_TICKS(ALARM_INTERVAL_MS));
            } 
            else if(Classification == "elevated")
            {
                digitalWrite(BUZZER_PIN,LOW);
                digitalWrite(RGB_R_PIN,LOW);
                digitalWrite(RGB_G_PIN,LOW);
                digitalWrite(RGB_B_PIN,HIGH);
                vTaskDelay(pdMS_TO_TICKS(ALARM_INTERVAL_MS));
            }
            else if(Classification == "fualt")
            {
                digitalWrite(BUZZER_PIN,HIGH);
                digitalWrite(RGB_R_PIN,HIGH);
                digitalWrite(RGB_G_PIN,LOW);
                digitalWrite(RGB_B_PIN,LOW);
                vTaskDelay(pdMS_TO_TICKS(ALARM_INTERVAL_MS));
            }
        }
    }
    xTaskDelete(NULL);

}
