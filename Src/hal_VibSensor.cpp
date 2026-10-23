#include"hal_VibSensor.h"


void VIB_SENSOR_init(gpio_num_t Sensor_Pin)
{
    pinMode(Sensor_Pin,INPUT);
}
void VIB_SENSOR_READ(gpio_num_t Sensor_Pin,float* Reading)
{
    for( ; ; )
    {
        Serial.println("Vibration Sensor is Reading....");
        *Reading=analogRead(Sensor_Pin);
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_INTERVALS_MS));
    }
    
    xTaskDelete(NULL);
}
