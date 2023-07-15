#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define DHTPIN 11     // Chân kết nối cảm biến DHT
#define DHTTYPE DHT11   // Loại cảm biến DHT

const int redLed = 13;       // Chân đèn LED kết nối với chân 13 trên Arduino
const int buzzerPin = 12;   // Chân buzzer kết nối với chân 12 trên Arduino

DHT dht(DHTPIN, DHTTYPE);

// Thời gian ấp trứng
static uint8_t dayTimeToHatch;
static uint8_t hourTimeToHatch;
static uint8_t minTimeToHatch;

// Nhiệt độ ấp trứng
static uint8_t setTemp = 20;
static float currentTemp; // currentTemp = dht.readTemperature()

// Trạng thái quá trình [0]: chờ, [1]: ấp, [2]: tiếp tục, [3]: set time hatch.
static uint8_t prState = 0;

uint8_t state = 0;

#define millisWaiting   1000              // 1s
#define millisHatching  1000              // 1s
#define millisBuzzer    1000*5            // 5s
#define millisAlarm     1000*25           // 25s

void updateDisplayTimeToHatch(uint8_t day, uint8_t hour, uint8_t min) {
  lcd.setCursor(6, 2);
  lcd.print("TimeHatch");
  lcd.setCursor(6, 3);
  if (day <= 9) {
    lcd.print("0");
    lcd.setCursor(7, 3);
  }
  lcd.print(day);
  lcd.setCursor(8, 3);
  lcd.print(":");
  lcd.setCursor(9, 3);
  if (hour <= 9) {
    lcd.print("0");
    lcd.setCursor(10, 3);
  }
  lcd.print(hour);
  lcd.setCursor(11, 3);
  lcd.print(":");
  lcd.setCursor(12, 3);
  if (min <= 9) {
    lcd.print("0");
    lcd.setCursor(13, 3);
  }
  lcd.print(min);
}

void updateDisplayTemp() {
  lcd.setCursor(16, 2);
  lcd.print("Temp");
  lcd.setCursor(17, 3);
  if (prState == 3)
    lcd.print(setTemp);
  else
    lcd.print((uint8_t)dht.readTemperature());
}

void tempAdjusting() {
  currentTemp = dht.readTemperature();

  if (currentTemp >= setTemp + 1) {
    digitalWrite(redLed, LOW);
  } else if (currentTemp <= setTemp - 1) {
    digitalWrite(redLed, HIGH);
  }
}

void sendData_th() {
  float currentTemp = dht.readTemperature();
  float currentHumidity = dht.readHumidity();

  if (isnan(currentTemp) || isnan(currentHumidity)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.println(currentTemp);
    Serial.println(currentHumidity);
  }
}

uint8_t processInputState(String input) {
  input.trim(); // Loại bỏ khoảng trắng đầu và cuối chuỗi

  if (input.startsWith("setTemp")) {
    int setTempIndex = input.indexOf("setTemp") + 7;
    setTemp = input.substring(setTempIndex).toInt();
    state = 1;
    return state; // Trả về giá trị trạng thái hiện tại
  }

  if (input.startsWith("prState")) {
    int prStateIndex = input.indexOf("prState") + 7;
    state = input.substring(prStateIndex).toInt();
    return state;
  }
}

void ringBuzzer() {
  const unsigned long buzzerInterval = 10 * 1000; // Thời gian chờ giữa các lần bật chuông (5 phút)
  const unsigned long buzzerDuration = 2 * 1000; // Thời gian chuông báo kéo dài (10 giây)

  static unsigned long previousBuzzerTime = 0; // Thời điểm lần cuối bật chuông
  static bool isBuzzerOn = false; // Trạng thái bật/tắt chuông

  unsigned long currentMillis = millis();

  // Kiểm tra điều kiện bật chuông
  if (!isBuzzerOn && currentMillis - previousBuzzerTime >= buzzerInterval) {
    // Bật chuông
    tone(buzzerPin, 100); // Tạo tín hiệu âm thanh với tần số 100Hz
    isBuzzerOn = true;
    previousBuzzerTime = currentMillis;
  }

  // Kiểm tra điều kiện tắt chuông
  if (isBuzzerOn && currentMillis - previousBuzzerTime >= buzzerDuration) {
    // Tắt chuông
    noTone(buzzerPin); // Tắt tín hiệu âm thanh
    isBuzzerOn = false;
  }
}


void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(redLed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  // sendData_th();
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  int turnOffBuzzer = 0;
  if (Serial.available()) {
    String input = Serial.readString();
    input.trim(); // Loại bỏ khoảng trắng đầu và cuối chuỗi

    if (input.startsWith("dayTimeToHatch")) {
      int dayTimeIndex = input.indexOf("dayTimeToHatch") + 14;
      int hourTimeIndex = input.indexOf("hourTimeToHatch") + 15;
      int minuteTimeIndex = input.indexOf("minuteTimeToHatch") + 17;
      int prStateIndex = input.indexOf("prState") + 7;

      day = input.substring(dayTimeIndex, hourTimeIndex).toInt();
      hour = input.substring(hourTimeIndex, minuteTimeIndex).toInt();
      min = input.substring(minuteTimeIndex, prStateIndex).toInt();
      state = input.substring(prStateIndex).toInt();

      Serial.println(day);
      Serial.println(hour);
      Serial.println(min);
      Serial.println(state);
    }

    if (input.startsWith("prState")) {
      int prStateIndex = input.indexOf("prState") + 7;
      state = input.substring(prStateIndex).toInt();
    }

    if (input.startsWith("setTemp")) {
      state = processInputState(input);
      tempAdjusting();
    }

    unsigned long previousMillis = millis();
    unsigned long interval = 1000; // Đơn vị milliseconds (1 giây)

    while (state == 1) {
      if (Serial.available()) {
        String inputState = Serial.readString();
        state = processInputState(inputState);
      }
      if (state == 0) {
        Serial.print(day);
        Serial.print(hour);
        Serial.print(min);
        Serial.print(state);
        break;
      }

      unsigned long currentMillis = millis();

      if (currentMillis % 2000 == 0) {
        // Gửi tín hiệu nhiệt độ và độ ẩm về Qt liên tục sau mỗi 2 giây
        // sendData_th();
      }
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (min == 0) {
          if (hour == 0) {
            if (day == 0) {
              state = 2;
              break;
            } else {
              day--;
              hour = 23;
              min = 59;
            }
          } else {
            hour--;
            min = 59;
          }
        } else {
          min--;
        }

        // Cập nhật hiển thị trên LCD
        updateDisplayTimeToHatch(day, hour, min);
        updateDisplayTemp();
        tempAdjusting();
      }
    }
  }
  Serial.print(day);
  Serial.print(hour);
  Serial.print(min);
  Serial.print("state");
  Serial.print(state);

if (state == 2 && turnOffBuzzer == 0) {
  uint8_t  stopBuzzer = 0;

  while (turnOffBuzzer == 0 && stopBuzzer ==0) {
    ringBuzzer();
    delay(500);

    if (Serial.available()) {
      String inputTurnBuzzer = Serial.readString();
      inputTurnBuzzer.trim(); // Loại bỏ khoảng trắng đầu và cuối chuỗi

      if (inputTurnBuzzer.startsWith("TurnOffBuzzer")) {
        turnOffBuzzer = 1; // Cập nhật giá trị của turnOffBuzzer
        stopBuzzer = 1;
        Serial.print("buzzer");
        Serial.println(stopBuzzer);
      }
    }
  }
}




  delay(500);
}
