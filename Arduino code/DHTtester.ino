#include "DHT.h"

#define DHTPIN 8     // Chân kết nối cảm biến DHT11
#define DHTTYPE DHT11   // Loại cảm biến DHT11

const int LED_PIN = 13;       // Chân đèn LED kết nối với chân 13 trên Arduino
const int speaker_pin = 11;   // Chân còi kết nối với chân 11 trên Arduino

double setTemperature = 0.0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 test!");

  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  pinMode(speaker_pin, OUTPUT);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.println(t);
    Serial.println(h);
  }

  if (Serial.available()) {
    String x = Serial.readString(); // Đọc byte dữ liệu từ cổng serial

    // Kiểm tra nếu nhận được giá trị "TurnOnLed"
    if (x == "TurnOnLed") {
      digitalWrite(LED_PIN, HIGH); // Bật đèn LED
    }

    // Kiểm tra nếu nhận được giá trị "TurnOffLed"
    if (x == "TurnOffLed") {
      digitalWrite(LED_PIN, LOW); // Tắt đèn LED
    }

    // Kiểm tra nếu nhận được giá trị nhiệt độ
    // Chuyển đổi giá trị nhiệt độ thành số thực và lưu vào biến setTemperature
  if (x.startsWith("SetTemperature")) {
      String temperatureValue = x.substring(14); // Lấy giá trị nhiệt độ từ chuỗi nhận được
      setTemperature = temperatureValue.toFloat(); // Chuyển đổi giá trị nhiệt độ thành số thực và lưu vào biến setTemperature

      if (t > setTemperature) {
        tone(speaker_pin, 255); // Phát âm thanh tại tần số 255 trên chân còi
        digitalWrite(LED_PIN, HIGH); // Bật đèn LED
      } else {
        digitalWrite(LED_PIN, LOW); // Tắt đèn LED
        noTone(speaker_pin); // Tắt âm thanh
      }
    }

    // Kiểm tra nếu nhận được giá trị "TurnOffSpeaker"
    if (x == "TurnOffSpeaker") {
      noTone(speaker_pin); // Tắt âm thanh
    }
  }

  delay(5000);
}
