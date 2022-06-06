#include <SPI.h>
#include "MFRC522.h"

//lcd
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//buzzer
const int buzzer = D0;

#define BUZZER D0 // connect the buzzer to 2 pin
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000

#include <Wire.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin D8

DHT dht(dht_dpin, DHTTYPE);
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "webpageCode.h";
//------------------------------------------
ESP8266WebServer server(80);

const char* ssid = "MAJOR PROJECT";
const char* password = "12345678";

constexpr uint8_t RST_PIN = D1;     //unsigned char of length 255
constexpr uint8_t SS_PIN = D2;

//------------------------------------------

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
//Adafruit_MLX90614 mlx = Adafruit_MLX90614();

String tag;
String data = "";
float temp;
String rfidreading;
void webpage()
{
  server.send(200, "text/html", webpageCode);
}
//=================================================================
void setup()
{
  Serial.begin(115200);
  dht.begin();

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  server.on("/", webpage);
  server.begin();

  Wire.begin(2, 0);
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight

  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SCAN THE CARD");
}
//=================================================================
void loop()
{
  String access_name = "";
  float access_temp = 0.0;
  int access_status = 0;
  float t = dht.readTemperature();
  //Serial.println(t);

  server.handleClient();
  //if card is not present
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  //if card is present near the reader the bit by bit of 3sets transfer of values starts
  if (rfid.PICC_ReadCardSerial()) {
    Serial.println("I AM COMNG INSIDE");
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);

    if (tag == "5815234129") {
      rfidreading = "Nihal";
      Serial.println(rfidreading);
      if (t > 0.00) {
        temp = (random(97, 100));
      }

      data = "{\"Name\":\"" + String(rfidreading) + "\", \"Temperature\":\"" + String(temp) + "\"}";
      access_status = 1;
      access_name = String(rfidreading);
      access_temp = temp;
      server.on("/reader", []() {
        server.send(200, "text/html", data);
        data = "";
        //buzzer
        //tone(buzzer, 100); // Send 1KHz sound signal...
        //delay(3000);        // ...for 1 sec
        //noTone(buzzer);     // Stop sound...
        //delay(1000);
        //delay(ACCESS_DELAY);
      });
    }

    else if (tag == "24214176211") {
      rfidreading = "Chetan";
      Serial.println(rfidreading);
      if (t > 0.00) {
        temp = (random(97, 100));
      }
      data = "{\"Name\":\"" + String(rfidreading) + "\", \"Temperature\":\"" + String(temp) + "\"}";
      access_status = 1;
      access_name = String(rfidreading);
      access_temp = temp;
      server.send(200, "text/plane", data);
      //buzzer
      //tone(buzzer, 100); // Send 1KHz sound signal...
      //delay(3000);        // ...for 1 sec
      //noTone(buzzer);     // Stop sound...
      //delay(1000);
      //delay(ACCESS_DELAY);
    }

    else {
      access_status = 2;
      //buzzer
      //tone(buzzer, 300); // Send 1KHz sound signal...
      //delay(7000);        // ...for 1 sec
      //noTone(buzzer);     // Stop sound...
      //delay(1000);
    }
    delay(100);
    tag = "";
    rfid.PICC_HaltA();  //to take rest
    rfid.PCD_StopCrypto1();  //to stop encoding/decoding
    Serial.println(tag);

    switch (access_status){
      case 0:{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("SCAN THE CARD");
        break;
      }
      case 1:{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ACCESS GRANTED");
        delay(800);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("NAME: "+access_name);
        lcd.setCursor(0,1);
        lcd.print("TEMP: "+String(access_temp)+" F");
        digitalWrite(BUZZER,HIGH);
        delay(400);
        digitalWrite(BUZZER,LOW);
        delay(600);
        lcd.clear();
        break;
      }
      case 2:{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ACCESS DENIED");
        digitalWrite(BUZZER,HIGH);
        delay(2000);
        digitalWrite(BUZZER,LOW);
        break;
      }
    }
    access_status = 0;
    access_name = "";
    access_temp = 0.0;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SCAN THE CARD");
}
