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
const int greenLed = 7;
const int buzzer = 12;   // Chân còi kết nối với chân 12 trên Arduino
const int middleButton = 9; 
const int rightButton = 10;
const int leftButton = 8;

DHT dht(DHTPIN, DHTTYPE);

//Timer counter to start
static uint8_t hourTimeToStart;
static uint8_t minTimeToStart;

// Duration of hatching
static uint8_t dayTimeToHatch;
static uint8_t hourTimeToHatch;
static uint8_t minTimeToHatch;
static uint8_t dayHatch;
static uint8_t hourHatch;
static uint8_t minHatch;

// Temp 
static uint8_t tempHatch;
static float currentTemp; // currentTemp = dht.readTemperature()

// Process state [0]: waiting, [1]: hatching, [2]: hatched.
static uint8_t prState = 0;

#define millisWaiting   1000              // 1s
#define millisHatching  1000              // 1s
#define millisBuzzer    1000*5            // 5s
#define millisAlarm     1000*25           // 25s

void updateDisplayTimeToHatch(uint8_t day, uint8_t hour, uint8_t min){
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

void updateDisplayTemp(){
  lcd.setCursor(16, 2);
  lcd.print("Temp");
  lcd.setCursor(17, 3);
  if (prState == 3)
    lcd.print(tempHatch);
  else
    lcd.print((uint8_t)dht.readTemperature());
}
void countDownToHatch(int prState, int day, int hour, int min) {
  unsigned long previousMillis = millis();
  unsigned long interval = 1000; // Đơn vị milliseconds (1 giây)
 
  while (prState == 1) {
    if (Serial.available() && prState ==0) {
      // Dừng đếm ngược nếu nhận được tín hiệu 1 từ Serial port
      break;
    }

    unsigned long currentMillis = millis();

    if (currentMillis % 2000 == 0) {
      // Gửi tín hiệu nhiệt độ và độ ẩm về Qt liên tục sau mỗi 5 giây
      sendData_th();
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (min == 0) {
        if (hour == 0) {
          if (day == 0) {
            prState = 2;
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

    if (prState == 0) {
      // Dừng đếm thời gian khi state = 0

      return;
    }
  }
}


void tempAdjusting() {
  currentTemp = dht.readTemperature();

  if (currentTemp >= tempHatch + 1) {
    digitalWrite(redLed, LOW);
  } else if (currentTemp <= tempHatch - 1) {
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

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(middleButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
}

void loop() {
  sendData_th();
  if (Serial.available()) {
    String input = Serial.readString();
    input.trim(); // Loại bỏ khoảng trắng đầu và cuối chuỗi
    //tachs chuoi
    if (input.startsWith("dayTimeToHatch") && input.indexOf("hourTimeToHatch") != -1 &&
        input.indexOf("minuteTimeToHatch") != -1 && input.indexOf("prState") != -1) {
      int dayTimeIndex = input.indexOf("dayTimeToHatch") + 14;
      int hourTimeIndex = input.indexOf("hourTimeToHatch") + 15;
      int minuteTimeIndex = input.indexOf("minuteTimeToHatch") + 17;
      int prStateIndex = input.indexOf("prState") + 7;

uint8_t dayTime = input.substring(dayTimeIndex, hourTimeIndex).toInt();
uint8_t hourTime = input.substring(hourTimeIndex, minuteTimeIndex).toInt();
uint8_t minuteTime = input.substring(minuteTimeIndex, prStateIndex).toInt();
uint8_t state = input.substring(prStateIndex).toInt();
Serial.println(dayTime);
Serial.println(hourTime);
Serial.println(minuteTime);
Serial.println(state);
//update man hinh LCD
      if (state == 1) {
        updateDisplayTimeToHatch(dayTime, hourTime, minuteTime);
        countDownToHatch(state, dayTime, hourTime, minuteTime);
      }
     else if (state == 0) {
        // Dừng đếm ngay lập tức
        updateDisplayTimeToHatch(dayTime, hourTime, minuteTime);
        

      }

    }
  }

  delay(1000);
}
