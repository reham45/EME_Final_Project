#pragma once
#define HAL_OLED_H
#include <Arduino.h>
#include "system_config.h"

#define GPIO_21 21
#define GPIO_22 22
#define GPIO_25 25
#define GPIO_26 26
#define GPIO_27 27
#define GPIO_33 33

#define OLED_SCL_PIN GPIO_22
#define OLED_SDA_PIN GPIO_21
#define BUZZER_PIN GPIO_25
#define RGB_R_PIN GPIO_33
#define RGB_G_PIN GPIO_26
#define RGB_B_PIN GPIO_27
/*OLED Handle*/
#define OLED_WIDTH       128
#define OLED_HEIGHT       64
#define OLED_ADDRESS     0x3C

#define DISPLAY_INTERVAL_MS 500
#define ALARM_INTERVAL_MS 200

void OLED_init(void);
void OLED_DISPLAY_TASK(void* pvParameters);

void Alarm_TASK_init(void);
void Alarm_Task(void* pvParameters);


