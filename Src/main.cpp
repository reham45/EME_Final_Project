#include <Arduino.h>
#include <freeRTOS.h>
#include "hal_Oled.h"
#include "system_data.h"
#define STACK_SIZE 3072
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
void setup()
{
    Serial.begin(115200);
    delay(1000);
    /*initialization*/
    VIB_SENSOR_init(SENSOR_PIN);
    mqtt_init();
    Alarm_TASK_init(BUZZ_PIN,RGB_G_PIN,RGB_B_PIN,RGB_R_PIN);


    /**********Algorithm ,how will work*******/

/*Task_1:Sensor_Task and check the Error */
xTaskCreatePinnedToCore(VIB_SENSOR_READ,"Vibration sensor is Reading...",STACK_SIZE, NULL,2, NULL, 1);
if(xTaskCreatePinnedToCore(VIB_SENSOR_READ,"Vibration sensor is Reading...",STACK_SIZE, NULL,2, NULL, 1)==pdFALSE)
{
    Serial.println("The Sensor Error");
}
/*Task_2:OLED_Display  and check the Error */
xTaskCreatePinnedToCore(OLED_DISPLAY,"OLED will DISPLAY...",STACK_SIZE, NULL, 3, NULL, 0);
if(xTaskCreatePinnedToCore(OLED_DISPLAY,"OLED will DISPLAY...",STACK_SIZE, NULL, 3, NULL, 0)==pdFALSE)
{
    Serial.println("The OLED Error");
}
/*Task_3:Alarm_Task  and check the Error */
xTaskCreatePinnedToCore(Alarm_Task,"Alarm_Task...",STACK_SIZE, NULL,3, NULL, 1);
if(xTaskCreatePinnedToCore(Alarm_Task,"Alarm_Task...",STACK_SIZE, NULL,3, NULL, 1)==pdFALSE)
{
    Serial.println("The OLED Error");
}
}



void loop()
{
    vTaskDelete();
}