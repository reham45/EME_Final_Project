#ifndef _HAL_OLED_H_
#define _HAL_OLED_H_

#define GPIO_21 21
#define GPIO_22 22
#define GPIO_25 25
#define GPIO_26 26
#define GPIO_27 27
#define GPIO_33 33

#define OLED_SCL_PIN GPIO_22
#define OLED_SDA_PIN GPIO_21
#define BUZZ_PIN GPIO_25
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
void OLED_TASK(void* Parameters);

void Alarm_TASK_init(gpio_num_t BUZZER_PIN,gpio_num_t NORMALSTATE,gpio_num_t ELEVATEDSTATE,gpio_num_t ALARMSTATE);
void Alarm_Task(void*pvParameters);


#endif