#ifndef _HAL_VIBSENSOR_H_
#define _HAL_VIBSENSOR_H_
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>

#define GPIO_34 34 
#define SENSOR_PIN GPIO_34
#define SAMPLE_INTERVALS_MS 10

typedef struct {
    float rawFeatureSample;     
    MachineState_t state;       
    char label[16];             
    uint8_t confidence;         
    uint32_t timestamp;         
    uint32_t inferenceCount;    
} VibrationData_t;

/* Global*/
extern QueueHandle_t xDisplayQueue;
extern QueueHandle_t xAlarmQueue;
extern QueueHandle_t xMqttQueue;
extern SemaphoreHandle_t xI2cMutex;
extern EventGroupHandle_t xSystemEvents;

#define BIT_SENSOR_READY  (1 << 0)
#define BIT_OLED_READY    (1 << 1)
#define BIT_ALARM_READY   (1 << 2)
#define BIT_MQTT_READY    (1 << 3)
#define ALL_SYNC_BITS     (BIT_SENSOR_READY | BIT_OLED_READY | BIT_ALARM_READY | BIT_MQTT_READY)

void VIB_SENSOR_init(gpio_num_t Sensor_Pin);
void VIB_SENSOR_READ(gpio_num_t Sensor_Pin,void* pvParameters);



#endif