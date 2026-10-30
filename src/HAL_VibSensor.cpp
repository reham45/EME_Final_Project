#include <esp_task_wdt.h>
#include "HAL_VibSensor.h"
#include "System_Config.h"
#include <Mariam_Hl-project-1_inferencing.h> 

//TinyML Functions
static int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{
    extern float global_vib_buffer[]; 
    memcpy(out_ptr, global_vib_buffer + offset, length * sizeof(float));
    return 0;
}

SystemState_t runInference(float* inputBuffer, size_t bufferSize) 
{
    SystemState_t result;
    ei_impulse_result_t ml_result = { 0 };
    
    signal_t signal;
    signal.total_length = bufferSize;
    signal.get_data = &raw_feature_get_data;

    // Copy the input buffer to the global buffer for inference
    EI_IMPULSE_ERROR res = run_classifier(&signal, &ml_result, false);
    
    if (res != EI_IMPULSE_OK) {
        result.stateEnum = NORMAL_STATE;
        strcpy(result.ml_state, "NORMAL");
        result.confidence = 0.0f;
        return result;
    }

    float max_value = 0;
    int predicted_index = 0;
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (ml_result.classification[ix].value > max_value) {
            max_value = ml_result.classification[ix].value;
            predicted_index = ix;
        }
    }

    result.confidence = (uint8_t)(max_value * 100.0f);
    
    const char* label = ml_result.classification[predicted_index].label;
    strncpy(result.ml_state, label, sizeof(result.ml_state) - 1);
    result.ml_state[sizeof(result.ml_state) - 1] = '\0';

 
    if (strcmp(label, "fault") == 0 || strcmp(label, "FAULT") == 0) {
        result.stateEnum = FAULT_STATE;
    } else if (strcmp(label, "elevated") == 0 || strcmp(label, "ELEVATED") == 0) {
        result.stateEnum = ELEVATED_STATE;
    } else {
        result.stateEnum = NORMAL_STATE;
    }

    return result;
}

/*Sensor Initialization*/
void VIB_SENSOR_init(void)
{
    pinMode(SENSOR_PIN, INPUT);
    analogReadResolution(12);
}
/*Sensor Read Task*/
void VIB_SENSOR_READ(void* pvParameters)
{
    xEventGroupWaitBits(xSystemEvents, ALL_SYNC_BITS, pdFALSE, pdTRUE, portMAX_DELAY);
    esp_task_wdt_add(NULL);

    uint32_t inferenceCounter = 0;

    for (;;)
    {
        //take samples each 10ms for 100ms and calculate RMS
        float sumSquared = 0;
        const int numSamples = 10;
        for (int i = 0; i < numSamples; i++) {
            float val = (float)analogRead(SENSOR_PIN);
            sumSquared += (val * val);
            vTaskDelay(pdMS_TO_TICKS(10)); 
        }
        
        //RMS Amplitude
        float rmsValue = sqrt(sumSquared / numSamples);

        //Inference
        SystemState_t infResult = runInference(rmsValue);

        // Prepare payload for queues
        VibrationData_t payload;
        payload.rawFeatureSample = rmsValue;
        payload.state = infResult.stateEnum;
        strncpy(payload.label, infResult.ml_state, sizeof(payload.label) - 1);
        payload.label[sizeof(payload.label) - 1] = '\0';
        payload.confidence = (uint8_t)infResult.confidence;
        payload.timestamp = millis();
        payload.inferenceCount = ++inferenceCounter;

        if (inferenceCounter % 10 == 0)
        {
            sendLog("[Core %d][TinyML] Inference #%lu | RMS: %.1f | Pred: %s (%u%%) | Stack Free: %u B\n", 
                    xPortGetCoreID(), payload.inferenceCount, payload.rawFeatureSample, payload.label, payload.confidence, uxTaskGetStackHighWaterMark(NULL));
        }

        /*Send to Queues (Non-blocking) */
        xQueueSend(xAlarmQueue, &payload, 0);
        xQueueSend(xDisplayQueue, &payload, 0);
        xQueueSend(xMqttQueue, &payload, 0);

        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_INTERVALS_MS));
    } 
    vTaskDelete(NULL);
}