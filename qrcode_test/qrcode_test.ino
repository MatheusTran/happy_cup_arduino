#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <qrcode.h>

// Button pins
#define BUTTON_PIN_RED       25 //red
#define BUTTON_PIN_YELLOW    33 // yellow

// U8G2 Constructor for SH1122 256x64 OLED. I took it from the website
U8G2_SH1122_256X64_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void displayMessage(const String message) {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 30);
  u8g2.print(message);
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

  qrcode_initText(&qrcode, qrcodeBytes, 3, ECC_LOW, "https://www.youtube.com/watch?v=dQw4w9WgXcQ");
  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) {
          drawSizedPixel(size * x + xOff, size * y, size);
        }
    }
  }
  u8g2.sendBuffer();
}

void setup() {
  // Initialize the display
  u8g2.begin();

  //make input pins
  pinMode(BUTTON_PIN_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RED, INPUT_PULLUP);

  // Set font and size
  u8g2.setFont(u8g2_font_ncenB14_tr);

  //displayMessage("this is a test")

}

void loop() {
  if (digitalRead(BUTTON_PIN_YELLOW) == LOW) {
    // Yellow button pressed  
    displayMessage("Yellow pressed");
  } else if (digitalRead(BUTTON_PIN_RED) == LOW) {
    displayMessage("Red Pressed");
  } else {
    displayQRCode("https://www.youtube.com/watch?v=dQw4w9WgXcQ",20, 2);
  }

}