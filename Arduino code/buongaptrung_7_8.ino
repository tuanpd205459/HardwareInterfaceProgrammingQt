

#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define DHTPIN 11     // Chân kết nối cảm biến DHT22
#define DHTTYPE DHT11   // Loại cảm biến DHT22

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

int intDayTimetoHatch;
int intHourTimetoHatch;
int intMinTimetoHatch;

// Temp 
static uint8_t tempHatch;
static float currentTemp; // currentTemp = dht.readTemperature();

// Process state [0]: waiting, [1]: hatching, [2]: hatched.
static uint8_t prState = 0;

#define millisWaiting   1000              // 1s
#define millisHatching  1000              // 1s
#define millisBuzzer    1000*5            // 5s
#define millisAlarm     1000*25           // 25s
// cap nhat thoi gian, nhan tin hieu tu Qt
void updateDisplayTimeToHatch(int dayHatch, int hourHatch, int minHatch){
 dayTimeToHatch = dayHatch;
 hourTimeToHatch = hourHatch;
 minTimeToHatch = minHatch;

  lcd.setCursor(6, 2);
  lcd.print("TimeHatch");
  lcd.setCursor(6, 3);
  if (dayTimeToHatch <= 9) {
    lcd.setCursor(6, 3);
    lcd.print("0");
    lcd.setCursor(7, 3);
  } 
  lcd.print(dayTimeToHatch);
  lcd.setCursor(8, 3);
  lcd.print(":");
  lcd.setCursor(9, 3);
  if (hourTimeToHatch <= 9) {
    lcd.setCursor(9, 3);
    lcd.print("0");
    lcd.setCursor(10, 3);
  } 
  lcd.print(hourTimeToHatch);
  lcd.setCursor(11, 3);
  lcd.print(":");
  lcd.setCursor(12, 3);
  if (minTimeToHatch <= 9) {
    lcd.setCursor(12, 3);
    lcd.print("0");
    lcd.setCursor(13, 3);
  } 
  lcd.print(minTimeToHatch);
}


void updateDisplayTemp(){
  lcd.setCursor(16, 2);
  lcd.print("Temp");
  lcd.setCursor(17, 3);
  if (prState == 3) lcd.print(tempHatch);
  else lcd.print((uint8_t)dht.readTemperature());
}
/*
void Display(int prState){
    if (prState < 2 ){
    lcd.setCursor(3, 0);
    lcd.print("BUONG AP TRUNG");
    lcd.setCursor(0, 1);
    lcd.print("====================");
    lcd.setCursor(5, 2);
    lcd.print("|");
    lcd.setCursor(5, 3);
    lcd.print("|");
    lcd.setCursor(14, 2);
    lcd.print("|");
    lcd.setCursor(14, 3);
    lcd.print("|");
//    updateDisplayTimeToHatch();
 //   updateDisplayTemp();
  }else if (prState < 3){
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("BUONG AP TRUNG");
    lcd.setCursor(4, 2);
    lcd.print("TRUNG DA NO!");
  }else{
    while(!digitalRead(middleButton)|| (prState==3))
    {
      lcd.setCursor(3, 0);
      lcd.print("BUONG AP TRUNG");
      lcd.setCursor(5, 2);
      lcd.print("WELLCOME!!!");
    }
    delay(500);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("BUONG AP TRUNG");
    lcd.setCursor(0, 1);
    lcd.print("====================");
    lcd.setCursor(5, 2);
    lcd.print("|");
    lcd.setCursor(5, 3);
    lcd.print("|");
    lcd.setCursor(14, 2);
    lcd.print("|");
    lcd.setCursor(14, 3);
    lcd.print("|");
}
}
*/

void tempAdjusting(){
  currentTemp = dht.readTemperature();
  if (currentTemp >= tempHatch + 1) digitalWrite(redLed, LOW);
  else if (currentTemp <= tempHatch - 1) digitalWrite(redLed, HIGH);
}
int* receiveData() {
  static int timeArray[4] = {0}; // Mảng để lưu trữ giá trị ngày, giờ và phút va prState
  if (Serial.available()) {
    String data = Serial.readString();
  
    // Tìm vị trí của các chuỗi bắt đầu của số
    int dayTimeIndex = data.indexOf("dayTimeToHatch") + 14;
    int hourTimeIndex = data.indexOf("hourTimeToHatch") + 15;
    int minuteTimeIndex = data.indexOf("minuteTimeToHatch") + 17;
    int prStateIndex = data.indexOf("prState") + 7;

    // Tách và lấy giá trị số từ chuỗi
    String dayTimeString = data.substring(dayTimeIndex, hourTimeIndex );
    String hourTimeString = data.substring(hourTimeIndex, minuteTimeIndex);
    String minuteTimeString = data.substring(minuteTimeIndex, prStateIndex );
    String SprState = data.substring(prStateIndex);

    // Chuyển đổi giá trị số từ chuỗi sang kiểu int
    int dayTime = dayTimeString.toInt();
    int hourTime = hourTimeString.toInt();
    int minuteTime = minuteTimeString.toInt();
    int prState = SprState.toInt();

    // Lưu giá trị vào mảng và trả về con trỏ tới mảng
    timeArray[0] = dayTime;
    timeArray[1] = hourTime;
    timeArray[2] = minuteTime;
    timeArray[3] = prState;
    return timeArray;
  }
  // Trả về con trỏ NULL nếu không có dữ liệu nhận được từ Serial
  return NULL;
}




void countDownToHatch(int prState, int day, int hour, int min) {
  unsigned long previousMillis = millis();
  unsigned long interval = 1000; // Đơn vị milliseconds (1 giây)

  while (prState == 1) {
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
          }
          else {
            day--;
            hour = 23;
            min = 59;
          }
        }
        else {
          hour--;
          min = 59;
        }
      }
      else {
        min--;
      }
      // Cập nhật hiển thị trên LCD ở đây
      updateDisplayTimeToHatch(day, hour, min);
      updateDisplayTemp();
      //  Display(prState);
      tempAdjusting();
    }
  }
}


void sendData_th(){
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
unsigned long currentMillis;
sendData_th();

if(Serial.available()){
    String x = Serial.readString(); 

int* receiveDataArray = receiveData();
  // Kiểm tra con trỏ có khác NULL hay không
Serial.print("dayTime: ");
Serial.println(receiveDataArray[0]);
Serial.print("hourTime: ");
Serial.println(receiveDataArray[1]);
Serial.print("minuteTime: ");
Serial.println(receiveDataArray[2]);
Serial.print("prState: ");
Serial.println(receiveDataArray[3]);

  if (receiveDataArray != NULL) {
    if(receiveDataArray[3] == 1){
    updateDisplayTimeToHatch(receiveDataArray[0], receiveDataArray[1], receiveDataArray[2]);
    countDownToHatch(receiveDataArray[3], receiveDataArray[0], receiveDataArray[1], receiveDataArray[2]);
 }
}
}


delay(5000);
}
