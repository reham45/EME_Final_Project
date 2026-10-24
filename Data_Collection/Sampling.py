import serial
import pandas as pd
import time

COM_PORT = 'COM3'      # متنسيش تعدلي البورت
BAUD_RATE = 115200
SAMPLES_LIMIT = 1000   # 1000 عينة = 10 ثواني بمعدل 100Hz
FILES_PER_CLASS = 5    # 5 ملفات لكل حالة (4 تدريب، 1 اختبار)

# إعدادات الحالات والنطاقات التقريبية عشان تفتكري وإنتِ بتظبطي المقاومة
classes = [
    {"name": "NORMAL", "range_hint": "Around 500 - 1500"},
    {"name": "ELEVATED", "range_hint": "Around 1800 - 2800"},
    {"name": "FAULT", "range_hint": "Above 3200"}
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

    for cls in classes:
        class_name = cls["name"]
        range_hint = cls["range_hint"]
        
        for i in range(1, FILES_PER_CLASS + 1):
            # تحديد نوع الملف (تدريب ولا اختبار)
            file_type = "train" if i <= 4 else "test"
            filename = f"{file_type}_{class_name}_{i}.csv"
            
            print("="*60)
            print(f"📁 NEXT FILE: {filename}")
            print(f"🎯 TARGET CLASS: {class_name} | Target Value: {range_hint}")
            print("-" * 60)
            
            # --- وضع المعاينة المباشرة (Live Preview) ---
            print("⚙️ Adjust the potentiometer now.")
            while True:
                ser.reset_input_buffer() # تفريغ البافر عشان نقرأ أحدث قيمة
                # قراءة كام سطر وتجاهلهم لضمان إننا بناخد قراءة كاملة مش مقطوعة
                for _ in range(3):
                    ser.readline()
                
                try:
                    fresh_val = ser.readline().decode('utf-8').strip()
                    print(f"📡 Current Sensor Value: {fresh_val}")
                except:
                    pass
                    
                user_cmd = input("-> Press [Enter] to refresh value, or type 's' + [Enter] to START recording: ")
                if user_cmd.lower() == 's':
                    break

            # --- وضع التسجيل (Recording Mode) ---
            print(f"\n🔴 Recording {SAMPLES_LIMIT} samples for {filename}...")
            data = []
            ser.reset_input_buffer()
            
            while len(data) < SAMPLES_LIMIT:
                try:
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        raw_value = int(line)
                        data.append(raw_value)
                        
                        # طباعة مؤشر كل 100 عينة عشان تحسي إن البرنامج شغال
                        if len(data) % 100 == 0:
                            print(f"[{len(data)}/{SAMPLES_LIMIT}] Collected... (Last read: {raw_value})")
                except ValueError:
                    pass 
                except KeyboardInterrupt:
                    print("\n⚠️ Session aborted by user.")
                    ser.close()
                    return

            # الحفظ في ملف CSV
            df = pd.DataFrame({
                'timestamp': [j * 10 for j in range(len(data))], # الزمن بالملي ثانية
                'raw_value': data
            })
            df.to_csv(filename, index=False)
            print(f"✅ Saved successfully: {filename}\n")

    ser.close()
    print("🎉 All 15 files have been collected! Ready for Edge Impulse.")

if __name__ == '__main__':
    collect_continuous_data()