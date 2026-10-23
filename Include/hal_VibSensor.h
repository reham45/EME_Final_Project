#ifndef _HAL_VIBSENSOR_H_
#define _HAL_VIBSENSOR_H_

#define GPIO_34 34 
#define SENSOR_PIN GPIO_34
#define SAMPLE_INTERVALS_MS 10

void VIB_SENSOR_init(gpio_num_t Sensor_Pin);
void VIB_SENSOR_READ(gpio_num_t Sensor_Pin,float* Reading);



#endif