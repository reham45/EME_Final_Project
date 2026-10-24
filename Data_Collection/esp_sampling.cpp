#include <Arduino.h>

#define VIBRATION_SENSOR_PIN 34 // عدلي الرقم بناءً على البن المتوصل عليه الحساس (يجب أن يكون Analog)

const unsigned long INTERVAL_MS = 10; // 10ms = 100Hz
unsigned long last_interval_ms = 0;

void setup() {
  Serial.begin(115200); // سرعة الاتصال (يجب أن تتطابق مع كود البايثون)
  analogReadResolution(12); // ESP32 يقرأ القيم من 0 إلى 4095
  delay(1000);
}

void loop() {
  unsigned long current_millis = millis();
  
  // تنفيذ القراءة كل 10 ملي ثانية بالضبط
  if (current_millis - last_interval_ms >= INTERVAL_MS) {
    last_interval_ms = current_millis;
    
    int raw_value = analogRead(VIBRATION_SENSOR_PIN);
    
    // طباعة القيمة فقط في سطر جديد ليلتقطها البايثون
    Serial.println(raw_value);
  }
}