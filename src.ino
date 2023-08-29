// #include "SD.h"
#include <FirebaseESP32.h>
#include <TFT_eSPI.h> 
#include <SPI.h>
#include "PMS.h"
#include <WiFiManager.h>

PMS pms(Serial);
PMS::DATA data;

FirebaseData firebaseData; 

TFT_eSPI tft = TFT_eSPI();

const unsigned char Connected_Wifi [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x1f, 0xff, 0x80, 0x3c, 
	0x03, 0xc0, 0x70, 0xf0, 0xe0, 0x23, 0xfe, 0x60, 0x0f, 0xdf, 0x00, 0x0c, 0x03, 0x80, 0x00, 0xf8, 
	0x00, 0x03, 0xfc, 0x00, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x70, 0x00, 
	0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char dis_WIFI [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfd, 0x80, 0x0f, 
	0xff, 0x80, 0x3c, 0x07, 0xc0, 0x31, 0xfc, 0xe0, 0x27, 0xfe, 0x40, 0x0f, 0x37, 0x80, 0x0c, 0x71, 
	0x00, 0x03, 0xfc, 0x00, 0x03, 0x8c, 0x00, 0x03, 0x00, 0x00, 0x06, 0x70, 0x00, 0x04, 0xf0, 0x00, 
	0x00, 0x70, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xD69A      /* 211, 211, 211 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F      /* 255, 192, 203 */ //Lighter pink, was 0xFC9F,0xFE19
#define TFT_BROWN       0x9A60      /* 150,  75,   0 */
#define TFT_GOLD        0xFEA0      /* 255, 215,   0 */
#define TFT_SILVER      0xC618      /* 192, 192, 192 */
#define TFT_SKYBLUE     0x867D      /* 135, 206, 235 */
#define TFT_VIOLET      0x915C      /* 180,  46, 226 */
#define ENCODER_A 34 // Pin for Encoder A
#define ENCODER_B 35 // Pin for Encoder B
#define Bt 32 
#define relay 16

volatile int encoder_value = 0;
unsigned long wait  = 1000;
unsigned long last = 0;
bool res;
int state;

  int _AQI_2_5;
  int _AQI_10_0;
  int final_AQI;

  //diff AQI2.5
  int X1 = -25;
  int X2 = 24;
  int X3 = 49;
  int X4 = 99;
  //diff MIN MAX pm 2.5
  int I1 = -25;
  int I2 = 11;
  int I3 = 12;
  int I4 = 39;
  //diff MIN MAX pm 10
  int Y1 = -50;
  int Y2 = 29;
  int Y3 = 39;
  int Y4 = 59;
WiFiManager wm;

// 128 x 160 px
void setup() {
  tft.init();
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  pinMode(Bt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoder_isr, CHANGE);
  // wm.resetSettings();f
  pinMode(relay , OUTPUT);
  Firebase.begin("https://iot-flutter-46ee1-default-rtdb.asia-southeast1.firebasedatabase.app/", "CWPnFFPpn6xb16sKCL4UufeofBUyEJg0H7NyM3Ts");
  tft.setRotation(1);
  Serial.begin(9600);
  start_page();
  Wifi_begin();
  delay(3000);
  encoder_value = -50;
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  // Particle();
  Selected_page();
    if ( Firebase.getString(firebaseData, "/ESP/Relay") ){
      String ledSState = firebaseData.stringData();
      if(ledSState=="1"){
            Serial.println(" ==> Led:1 Action");
            digitalWrite(relay,HIGH);
        }
        else{
          Serial.println(" ==> Led:0 Action");
          digitalWrite(relay,LOW);
        }
    }
  delay(500);
}

void start_page(){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(130,100,1);
  tft.setTextColor(TFT_WHITE);
  // tft.print(WiFi.RSSI());
  tft.drawWedgeLine(0, 20, 160, 20, 1, 1, TFT_WHITE);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 32, 4);
  tft.println("Dust Tracker");
  tft.setCursor(30, 70, 2);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.println("Debsirin School");
  tft.drawRoundRect(0, 0, 160, 127, 3, TFT_WHITE);
  }

void dispText(String text,int TextX, int TextY ,int color){
  tft.setTextColor(color);
  tft.setCursor(TextX,TextY,2);
  tft.print(text);
}
void drawvalue(int valuesensor ,int ValueX ,int ValueY){
  tft.setTextColor(TFT_SKYBLUE);
  tft.setCursor(ValueX,ValueY,2);
  tft.print(valuesensor);
}
void Particle(){
  if (pms.read(data))
  {
  tft.fillScreen(TFT_BLACK);
  wifi_check();
  Thai_AQI(data.PM_AE_UG_2_5,data.PM_AE_UG_10_0);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(5,5,4);
  tft.print("AQI : ");
  tft.setCursor(65,5,4);
  tft.print(final_AQI);
  tft.drawRoundRect(5, 41, 150, 80, 6, TFT_WHITE);
  dispText("PM  10 :",13,46,TFT_SKYBLUE);
  dispText("PM 2.5 :",13,73,TFT_SKYBLUE);
  dispText("PM 1.0 :",13,100,TFT_SKYBLUE);
  dispText("Main",120,23,TFT_SKYBLUE);
  sent_valueParticle(String(data.PM_AE_UG_10_0),String(data.PM_AE_UG_1_0),String(data.PM_AE_UG_2_5),String(final_AQI));
    if(encoder_value > 1){
    encoder_value = 1;
  }
  if(encoder_value < -1){
    encoder_value = -1;
  }
  int selected = map(encoder_value,-1,1,0,1);
  if(selected == 1){
    dispText("Main",120,23,TFT_ORANGE);
  }
  if (digitalRead(Bt) == 0 && selected == 1){
    delay(1000);
    state = 0;
  }
    Serial.println("Encoder value: " + String(encoder_value));
    if(digitalRead(Bt) == 0){
    Serial.println("Button press");
  }
  // pms.read(data);
  tft.setTextColor(TFT_SKYBLUE);
  tft.setCursor(80,46,2);
  tft.print(data.PM_AE_UG_1_0);
  tft.setCursor(110,46,2);
  tft.print("ug/m3");

  tft.setCursor(80,73,2);
  tft.print(data.PM_AE_UG_2_5);
  tft.setCursor(110,73,2);
  tft.print("ug/m3");


  tft.setCursor(80,100,2);
  tft.print(data.PM_AE_UG_10_0);
  tft.setCursor(110,100,2);
  tft.print("ug/m3");


  }
}
void Wifi_begin(){
if (WiFi.status() != WL_CONNECTED){
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(7, 3, 2);
        tft.print("Failed to connect");
        tft.drawBitmap(135,0,dis_WIFI,20,20,TFT_RED);
        // wm.autoConnect("Particle meter","admin123");
  }

    wm.autoConnect("Particle meter","admin123");
  if (WiFi.status() == WL_CONNECTED){
        start_page();
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(7, 3, 2);
        tft.print("connected"); 
        tft.drawBitmap(135,0,Connected_Wifi,20,20,TFT_WHITE); 
    }
    else{
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(7, 3, 2);
        tft.print("Failed to connect");
        tft.drawBitmap(135,0,dis_WIFI,20,20,TFT_RED);
        // wm.autoConnect("Particle meter","admin123");
  }
}
void wifi_check(){
    if (WiFi.status() == WL_CONNECTED){
        tft.drawBitmap(135,0,Connected_Wifi,20,20,TFT_WHITE); 
    }else{
        tft.drawBitmap(135,0,dis_WIFI,20,20,TFT_RED);
    }
}
void encoder_isr() {
  // Reading the current state of encoder A and B
  int A = digitalRead(ENCODER_A);
  int B = digitalRead(ENCODER_B);
  // If the state of A changed, it means the encoder has been rotated
  if ((A == HIGH) != (B == LOW)) {
    encoder_value--;
  } else {
    encoder_value++;
  }

}
void AQI2_5(int pm2_5){
  if(pm2_5 > 0 && pm2_5 < 26){
    _AQI_2_5 = ((X1/I1)*(pm2_5-0))+0;
  }else if(pm2_5 >= 26 && pm2_5 < 38){
    _AQI_2_5 = ((X2/I2)*(pm2_5-26))+26;
  }else if(pm2_5 >= 38 && pm2_5 < 51){
    _AQI_2_5 = ((X3/I3)*(pm2_5-38))+51;
  }else if(pm2_5 >= 51 && pm2_5 < 91){
    _AQI_2_5 = ((X4/I4)*(pm2_5-51))+101;
  }else if(pm2_5 >= 92){
    Serial.print("danger AQI > 200");
  }
  return;
}
void AQI10_0(int pm10_0){
    if(pm10_0 > 0 && pm10_0 < 51){
    _AQI_10_0 = ((X1/Y1)*(pm10_0-0))+0;
  }else if(pm10_0 >= 51 && pm10_0 < 81){
    _AQI_10_0 = ((X2/Y2)*(pm10_0-26))+26;
  }else if(pm10_0 >= 81 && pm10_0 < 121){
    _AQI_10_0 = ((X3/Y3)*(pm10_0-38))+51;
  }else if(pm10_0 >= 121 && pm10_0 < 181){
    _AQI_10_0 = ((X4/Y4)*(pm10_0-51))+101;
  }else if(pm10_0 >= 181){
    Serial.print("danger AQI > 200");
  }
  return;
}
void Thai_AQI(int PM2_5,int PM10){
  AQI10_0(PM10);
  AQI2_5(PM2_5);
  if(_AQI_10_0 >= _AQI_2_5){
    final_AQI = _AQI_10_0;
  }else{
    final_AQI = _AQI_2_5;
  }
  return;
}
void Selected_page(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  wifi_check();
  tft.drawRect(10, 35, 65, 40, TFT_WHITE);
  tft.drawRect(80, 35, 65, 40, TFT_WHITE);
  tft.drawRect(10, 80, 65, 40, TFT_WHITE);
  tft.drawRect(80, 80, 65, 40, TFT_WHITE);
  int page_num = map(encoder_value,-10,10,1,4);
  dispText("AQI",30,45,TFT_SKYBLUE);
  // delay(500);

  if(encoder_value > 10){
    encoder_value = 10;
  }
  if(encoder_value < -10){
    encoder_value = -10;
  }


  if(page_num == 1){
    tft.drawRect(10, 35, 65, 40, TFT_ORANGE);
    // delay(200);
    while(state == 1){
      Particle();
    }
  }else if(page_num == 2){
    tft.drawRect(80, 35, 65, 40, TFT_ORANGE);
  }else if(page_num == 3){
    tft.drawRect(10, 80, 65, 40, TFT_ORANGE);
  }else if(page_num == 4){
    tft.drawRect(80, 80, 65, 40, TFT_ORANGE);
  }
  // tft.drawNumber(encoder_value,0,0,2)
  if(digitalRead(Bt) == 0 && page_num == 1){
    state = 1;
  }
  


  Serial.println("Encoder value: " + String(encoder_value));
    if(digitalRead(Bt) == 0){
    Serial.println("Button press");
  }
}
void sent_valueParticle(String value1,String value2,String value3,String value4){
 if( millis() - last > wait) {
     last = millis(); 
     Firebase.setString(firebaseData, "/ESP/PM 10", value1);
     Firebase.setString(firebaseData, "/ESP/PM 1_0", value2);
     Firebase.setString(firebaseData, "/ESP/PM 2_5", value3);
     Firebase.setString(firebaseData, "/ESP/AQI", value4);
 }
}
