#include <esp_task_wdt.h>
#include"hal_VibSensor.h"
/*#include "tinyml_model.h"*/

extern VibrationData_t runInference(float current_rms);

void VIB_SENSOR_init(gpio_num_t Sensor_Pin)
{
    pinMode(Sensor_Pin,INPUT);
    analogReadResolution(12);
}

void VIB_SENSOR_READ(gpio_num_t Sensor_Pin,void* pvParameters)
{
    xEventGroupWaitBits(xSystemEvents, ALL_SYNC_BITS, pdFALSE, pdTRUE, portMAX_DELAY);
    esp_task_wdt_add(NULL);
/*********************************/

    uint32_t inferenceCounter = 0;
    float featureBuffer[TINYML_FEATURE_COUNT];

    for (;;)
    {
        /* 1. Collect features for model input window */
        for (int i = 0; i < TINYML_FEATURE_COUNT; i++)
        {
            featureBuffer[i] = (float)analogRead(SENSOR_PIN);
            vTaskDelay(pdMS_TO_TICKS(SAMPLE_INTERVALS_MS));
        }  
    /* 2. TINYML INFERENCE CODE (Commented out for future library integration)
    * -----------------------------------------------------------------
    * signal_t features_signal;
    * numpy::signal_from_buffer(featureBuffer, TINYML_FEATURE_COUNT, &features_signal);
    * ei_impulse_result_t result = { 0 };
    * run_classifier(&features_signal, &result, false);
    * 
    * // Parse probabilities from result.classification[]
    * ----------------------------------------------------------------- */
    float latestSample = featureBuffer[TINYML_FEATURE_COUNT - 1];

    VibrationData_t payload;
    payload.rawFeatureSample = latestSample;
    payload.timestamp = millis();
    payload.inferenceCount = ++inferenceCounter;

if (inferenceCounter % 10 == 0)
        {
            Serial.printf("[Core %d][TinyML] Inference #%lu | Pred: %s (%u%%) | Stack Free: %u B\n",xPortGetCoreID(), payload.inferenceCount, payload.label,payload.confidence, uxTaskGetStackHighWaterMark(NULL));
        }

       
        /* Dispatch payloads via FreeRTOS Queues (Non-blocking) */
        xQueueSend(xAlarmQueue, &payload, 0);
        xQueueSend(xDisplayQueue, &payload, 0);
        xQueueSend(xMqttQueue, &payload, 0);

        esp_task_wdt_reset();
    } 
    vTaskDelete(NULL);
}
