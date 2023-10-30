#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// defining pins
#define BUTTON_PIN_RED       25 //red
#define BUTTON_PIN_YELLOW    33 // yellow
#define SERVO_PIN_DISC       13   
#define SERVO_PIN_CLAW       14
//#define SCL_PIN              
//#define SS_PIN_OLED      SPI automatically does it
#define RST_PIN           26
#define SS_PIN_RFID_CARD  32
#define SS_PIN_RFID_CUP   5
#define MISO_PIN          19
#define MOSI_PIN          23
#define SCK_PIN           18

U8G2_SH1122_256X64_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char* ssid = "Mats-hotspot";
const char* password = "quackquack";

const String api_end_point = "https://happycup.iran.liara.run/api/dispense/12345678?cup_id=1&uni_id=1";


bool isYellowPressed = false;
bool isRedPressed = false;

void displayMessage(const String message) {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 30);
  u8g2.print(message);
  u8g2.sendBuffer();
}

void displayQRCode(String text) {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 20);
  u8g2.print("QR Code:");

  // Generate the QR code

  // Display the QR code on the screen
  u8g2.drawRFrame(20, 30, 80, 80, qrcode.getModuleSize(), 1);
  u8g2.sendBuffer();

  Serial.println("QR Code displayed.");
}

String sendRequest() {
  HTTPClient http;
  http.useHTTP10(true);
  http.begin(api_end_point);
  http.GET();
  String result = http.getString();

  //convert it to json format
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, result);

  //I currently don't handle the json stuff. and just return it as a string
  return result;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RED, INPUT_PULLUP);
  u8g2.begin();

  // Set font and size
  u8g2.setFont(u8g2_font_ncenB14_tr);

  //wifi stuff
  WiFi.begin(ssid, password, 6);
  displayMessage("connecting to network");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    u8g2.print(".");
    u8g2.sendBuffer();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
if (digitalRead(BUTTON_PIN_YELLOW) == LOW || isYellowPressed) {
    // Yellow button pressed
    isYellowPressed = true;
    isRedPressed = false;
    String result = sendRequest();
    displayMessage(result);
  } else if (digitalRead(BUTTON_PIN_RED) == LOW || isRedPressed) {
    // Red button pressed
    isRedPressed = true;
    isYellowPressed = false;
    displayQRCode(api_end_point);
  } 
}
