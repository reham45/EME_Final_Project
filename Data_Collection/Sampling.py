import serial
import pandas as pd
import time

COM_PORT = 'COM3'      
BAUD_RATE = 115200
SAMPLES_LIMIT = 1000    # 1000 عينة = 10 ثوانٍ بمعدل 100Hz

# ... (نفس مصفوفة tasks الخاصة بك) ...

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
        print(f"ACTION REQUIRED: Adjust to {class_name} ({range_hint})")
        input("Press [ENTER] when ready to start recording...")
        
        data = []
        ser.reset_input_buffer()
        
        while len(data) < SAMPLES_LIMIT:
            try:
                line = ser.readline().decode('utf-8').strip()
                if line:
                    raw_value = int(line) # تحويل القراءة الخام
                    data.append(raw_value)
                    if len(data) % 100 == 0: # طباعة كل 100 عينة لتقليل العبء
                        print(f"[{len(data)}/{SAMPLES_LIMIT}] Collected...")
            except ValueError:
                pass 
            except KeyboardInterrupt:
                print("\nSession aborted.")
                ser.close()
                return

        # Edge Impulse يفضل وجود عمود للزمن بالملي ثانية
        df = pd.DataFrame({
            'timestamp': [i * 10 for i in range(len(data))], # 0, 10, 20, 30 ms...
            'raw_value': data
        })
        df.to_csv(filename, index=False)
        print(f"✅ Saved successfully: {filename}\n")
        
    ser.close()
    print("🎉 All 9 files collected!")

if __name__ == '__main__':
    collect_continuous_data()