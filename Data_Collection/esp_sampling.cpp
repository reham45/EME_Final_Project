#include <Arduino.h>

const int POT_PIN = 34; 
const int DELAY_MS = 10; // 100Hz Sampling Rate (1 sample every 10ms)

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // ADC 0-4095
  delay(1000);
}

void loop() {
  
  int raw_value = analogRead(POT_PIN);
  Serial.println(raw_value);
  
  delay(DELAY_MS); 
}