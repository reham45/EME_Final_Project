import serial
import pandas as pd
import time

COM_PORT = 'COM3'      
BAUD_RATE = 115200
SAMPLES_LIMIT = 100    # 100 عينة للملف (20 ثانية فقط)

# قائمة الـ 9 ملفات: 2 للتدريب و 1 للاختبار من كل حالة
tasks = [
    ("NORMAL", "dataset_NORMAL_train1.csv", "Under 1500"),
    ("NORMAL", "dataset_NORMAL_train2.csv", "Under 1500"),
    ("NORMAL", "dataset_NORMAL_test.csv", "Under 1500"),
    ("ELEVATED", "dataset_ELEVATED_train1.csv", "Between 1500 and 3000"),
    ("ELEVATED", "dataset_ELEVATED_train2.csv", "Between 1500 and 3000"),
    ("ELEVATED", "dataset_ELEVATED_test.csv", "Between 1500 and 3000"),
    ("FAULT", "dataset_FAULT_train1.csv", "Above 3000"),
    ("FAULT", "dataset_FAULT_train2.csv", "Above 3000"),
    ("FAULT", "dataset_FAULT_test.csv", "Above 3000")
]

def collect_continuous_data():
    print(f"Connecting to {COM_PORT}...")
    try:
        ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=2)
        time.sleep(2)
        print("Connected successfully!\n")
    except Exception as e:
        print(f"Error connecting: {e}")
        return

    for class_name, filename, range_hint in tasks:
        print("="*50)
        print(f"NEXT FILE: {filename}")
        print(f"ACTION REQUIRED: Turn potentiometer to {class_name} ({range_hint})")
        
        input("Press [ENTER] when ready to start recording...")
        
        print(f"Recording {SAMPLES_LIMIT} samples...")
        data = []
        ser.reset_input_buffer()
        
        while len(data) < SAMPLES_LIMIT:
            try:
                line = ser.readline().decode('utf-8').strip()
                if line:
                    rms_value = float(line)
                    data.append(rms_value)
                    print(f"[{len(data)}/{SAMPLES_LIMIT}] RMS: {rms_value}")
            except ValueError:
                pass 
            except KeyboardInterrupt:
                print("\nSession aborted by user.")
                ser.close()
                return

        df = pd.DataFrame({'timestamp': range(len(data)), 'rms_value': data})
        df.to_csv(filename, index=False)
        print(f"✅ Saved successfully: {filename}\n")
        
    ser.close()
    print("🎉 All 9 files have been collected and saved! Great job.")

if __name__ == '__main__':
    collect_continuous_data()