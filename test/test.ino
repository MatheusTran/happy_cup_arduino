#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

// U8G2 Constructor for SH1122 256x64 OLED. I took it from the website
U8G2_SH1122_256X64_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Define MFRC522 pins
#define RST_PIN    26   // Reset pin
#define SS_PIN     5   // Slave Select pin
#define BUTTON_PIN_RED       25 //red
#define BUTTON_PIN_YELLOW    33 // yellow

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

bool isYellowPressed = false;
bool isRedPressed = false;

void displayMessage(const String message) {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 30);
  u8g2.print(message);
  u8g2.sendBuffer();
}



void setup() {
  // Initialize the display
  u8g2.begin();
  
  // Set font and size
  u8g2.setFont(u8g2_font_ncenB14_tr);
    
  //make input pins
  pinMode(BUTTON_PIN_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RED, INPUT_PULLUP);

  // Initialize the MFRC522 reader
  SPI.begin();        // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522

  Serial.begin(9600);
  Serial.println("Waiting for card...");
}

void loop() {
  if (digitalRead(BUTTON_PIN_YELLOW) == LOW) {
    displayMessage("Yellow pressed");
    Serial.println("Yellow pressed");
  } else if (digitalRead(BUTTON_PIN_RED) == LOW) {
    displayMessage("Red pressed");
    Serial.println("Red Pressed");
  } else {
    displayMessage("Red to dispense, Yellow to return");
  }
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Card detected, display UID
    u8g2.clearBuffer();
    u8g2.setCursor(0, 30);
    u8g2.print("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      u8g2.print(" ");
      u8g2.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      u8g2.print(mfrc522.uid.uidByte[i], HEX);
    }
    u8g2.sendBuffer();
    Serial.println("Card detected! UID:");
    mfrc522.PICC_HaltA(); // Halt the card
  }
  delay(500);
}