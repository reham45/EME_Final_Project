#include <Arduino.h>
#include <math.h>

const int POT_PIN = 34; // Analog pin connected to the potentiometer[cite: 59, 82]
const int SAMPLES_COUNT = 10;
const int DELAY_MS = 10; // 100Hz Sampling Rate (1000ms / 100 = 10ms)

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Set ADC resolution to 12-bit (0-4095)
  delay(1000);
}

void loop() {
  float sum_of_squares = 0;
  float current_adc = 0;

  // 1. Rapid Sampling: Collect 10 samples
  for (int i = 0; i < SAMPLES_COUNT; i++) {
    current_adc = analogRead(POT_PIN);
    sum_of_squares += (current_adc * current_adc);
    delay(DELAY_MS);
  }

  // 2. Feature Extraction: Calculate RMS (Root Mean Square)
  float rms = sqrt(sum_of_squares / SAMPLES_COUNT);

  // 3. Print the RMS value for the Python script to read
  Serial.println(rms);

  // Small delay to prevent Serial Buffer overflow during data collection
  delay(100); 
}