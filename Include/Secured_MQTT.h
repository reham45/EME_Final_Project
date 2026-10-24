#ifndef _SECURED_MQTT_H_
#define _SECURED_MQTT_H_

#include <Arduino.h>
#include "hal_VibSensor.h"

#define WIFI_SSID           "YOUR_WIFI_SSID"
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"

#define MQTT_BROKER_URL     "your-cluster.s1.eu.hivemq.cloud"
#define MQTT_TLS_PORT       8883
#define MQTT_USERNAME       "your_hivemq_user"
#define MQTT_PASSWORD       "your_hivemq_password"
#define MQTT_TOPIC_PUBLISH  "industrial/tinyml/vibration"

#define MQTT_PUBLISH_PERIOD_MS 1000

void mqtt_init(void);
void vMqttTask(void *pvParameters);

#endif /* _SECURED_MQTT_H_ */