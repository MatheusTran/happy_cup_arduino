// libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <qrcode.h>

// defining pins
// Button pins
#define BUTTON_PIN_RED       25 //red
#define BUTTON_PIN_YELLOW    33 // yellow

// Motor pins
#define SERVO_PIN_DISC       13   // brown
#define SERVO_PIN_CLAW       14   // blue
#define DISC_SPEED           40   // speed
#define START_POSITION       28   // starting position of claw
#define OPEN_ANGLE           10  // open angle of claw

// RFID reader pins 
#define RST_PIN           26
#define SS_PIN_RFID_CARD  32
#define SS_PIN_RFID_CUP   5
#define MISO_PIN          19
#define MOSI_PIN          23
#define SCK_PIN           18

// OLED screen pins:
#define SCL_PIN           22
#define SDA_PIN           21


// location pins
//#define NFC_LOC_PIN 
//#define DROPDOWN_LOC_PIN 
//#define IN_OUT_LOC_PIN 


bool isYellowPressed = false;
bool isRedPressed = false;
unsigned long cardUID = 0; // Store the card UID as an unsigned long

//Wifi network and password
const char* ssid = "Mats-hotspot";
const char* password = "quackquack";

//API end point
char* dispense_api = "https://happycup.iran.liara.run/api/dispense/12345678?uni_id=";
char* return_api = "https://happycup.iran.liara.run/api/return/12345678?uni_id=";


// defining objects
//OLED display
U8G2_SH1122_256X64_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// servo motors
Servo servo_disc;
Servo servo_claw;
// RFID readers
MFRC522 mfrc522_card(SS_PIN_RFID_CARD, RST_PIN);
MFRC522 mfrc522_cup(SS_PIN_RFID_CUP, RST_PIN);

//functions

void displayMessage(String message, int cutoff) {
  u8g2.clearBuffer();
  const char* text = message.c_str();
  u8g2.setCursor(0, 30);
  if (message.length() < cutoff) {
    u8g2.print(message);
    u8g2.sendBuffer();
    return;
  }
  for (int i = 0; i < cutoff; i++) {
    u8g2.print(text[i]);
  }
  u8g2.setCursor(0, 60);
  for (int i = cutoff; i < message.length(); i++) {
    u8g2.print(text[i]);
  }
  u8g2.sendBuffer();
}

void drawSizedPixel(int x, int y, int size) {
  for (int yOff = 0; yOff < size; yOff++) {
    for (int xOff = 0; xOff < size; xOff++) {
      u8g2.drawPixel(x + xOff, y + yOff);
    }
  }
}

void displayQRCode(String text, int xOff, int size) {
  u8g2.clearBuffer();
  QRCode qrcode;
  
  // Allocate a chunk of memory to store the QR code
  uint8_t qrcodeBytes[qrcode_getBufferSize(3)];


  qrcode_initText(&qrcode, qrcodeBytes, 3, ECC_LOW, text.c_str());
  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) {
          drawSizedPixel(size * x + xOff, size * y, size);
        }
    }
  }
  u8g2.sendBuffer();
}

bool sendRequest(bool return_cup, String user_id, String cup_id) {
  // sending request
  HTTPClient http;
  http.useHTTP10(true);
  String endpoint = "";
  if (return_cup) {
    endpoint = return_api + user_id;
    http.begin(endpoint.c_str());
    http.GET();
    String result = http.getString();
    return true;
  } else {
    endpoint = dispense_api + user_id;
    http.begin(endpoint.c_str());
  }
  http.GET();
  String result = http.getString();
  bool valid = false;
  //convert it to json format
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, result);
  String status = doc["status"];
  displayMessage(doc["message"], 28);
  if (status == "success") {
    valid = true;
  } else {
    delay(4000);
    displayQRCode(doc["url"], 0, 2);
    delay(11000);
  } 
  delay(4000);

  //I currently don't handle the json stuff. and just return it as a string
  return valid;
}

void setup() {
  // initialise servo motors
  servo_disc.setPeriodHertz(50);
	servo_disc.attach(SERVO_PIN_DISC, 500, 2500);
  servo_claw.setPeriodHertz(50);
	servo_claw.attach(SERVO_PIN_CLAW, 500, 2500);
  
  //make input pins
  pinMode(BUTTON_PIN_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RED, INPUT_PULLUP);

  // Initialize the display
  u8g2.begin();

  // Set font and size
  u8g2.setFont(u8g2_font_ncenB14_tr);


  // Initialize the MFRC522 reader
  SPI.begin();        // Initialize SPI bus
  mfrc522_card.PCD_Init(); // Initialize MFRC522
  mfrc522_cup.PCD_Init(); // Initialize MFRC522

  Serial.begin(115200);

  //Set up wifi
  WiFi.begin(ssid, password, 6);
  displayMessage("connecting to network", 28);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    u8g2.print(".");
    u8g2.sendBuffer();
  }
  displayMessage("Connected to network", 28);
  servo_claw.write(START_POSITION);
}

String scan_card(MFRC522 mfrc522) {
  String uid = "";
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i], HEX); 
    }
    mfrc522.PICC_HaltA(); // Halt the card
  }
  return uid;
}

void release_cup() {
    servo_claw.write(START_POSITION + OPEN_ANGLE);
    delay(400);
    servo_claw.write(START_POSITION);
}

void dispense_sequence() {
  displayMessage("Getting cup...", 28);
  release_cup();
  delay(1000);
  servo_disc.write(90 + DISC_SPEED);
  delay(3600);
  servo_disc.write(90);
  delay(1000);
  servo_disc.write(90 - DISC_SPEED);
  delay(3600);
  servo_disc.write(90);
  displayMessage("Enjoy your cup ;)", 28);
  delay(2000);
}

bool cup_scan() {
  bool loop = true;
  bool is_valid = false;
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    String cup = scan_card(mfrc522_cup);
    if (cup.compareTo("") > 0) {
      //is_valid = sendRequest(true, "1", "1");
      is_valid = true;
      break;
    }
  }
  return is_valid;
}

bool return_sequence() {
  servo_disc.write(90 + DISC_SPEED);
  delay(3200); //waiting for positional switch
  servo_disc.write(90);
  
  bool is_valid_cup = cup_scan();
  if (is_valid_cup) {
    displayMessage("Thank you!!! :)", 28);
    servo_disc.write(90 + DISC_SPEED);
    delay(4600);
    servo_disc.write(90);
    return true;
  } 
  displayMessage("invalid cup >:(", 28);
  servo_disc.write(90 - DISC_SPEED);
  delay(3200);
  servo_disc.write(90);
  return false;
}


void loop() {
  // Check button states
  if (digitalRead(BUTTON_PIN_YELLOW) == LOW || isYellowPressed) {
    // Yellow button pressed
    isYellowPressed = true;
    isRedPressed = false; 
    displayMessage("Dispense: Waiting for card...", 28);
  } else if (digitalRead(BUTTON_PIN_RED) == LOW || isRedPressed) {
    isRedPressed = true;
    isYellowPressed = false;
    displayMessage("Return: Waiting for card...", 28);
  } else {
    displayMessage("Red to return Yellow to dispense", 14);
  }
  if (isYellowPressed || isRedPressed) {
    unsigned long startTime = millis();
    while (millis() - startTime < 3000) {
      String uid = scan_card(mfrc522_card);
      if (uid.compareTo("") > 0) {
        if (isRedPressed) {
          displayMessage("validating cup", 28);
          
          bool returned_cup = return_sequence();
          if (returned_cup) {
            sendRequest(true, uid, ""); 
          }
        } else {
          displayMessage("checking credits", 28);
          bool sufficient_credits = sendRequest(false, uid, ""); 
          if (sufficient_credits) {
            dispense_sequence();
          }
        }
        isYellowPressed = false;
        isRedPressed = false;
        break;
      }
    }
  }
  // Look for new cards
  delay(100); // Delay for smoother display update
}