// libraries
#include <Wire.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>


// defining pins
#define SERVO_PIN_DISC       13   
#define SERVO_PIN_CLAW       14

Servo servo_disc;
Servo servo_claw;

U8G2_SH1122_256X64_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void displayMessage(const String message) {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 30);
  u8g2.print(message);
  u8g2.sendBuffer();
}

//
bool isYellowPressed = false;
bool isRedPressed = false;


void setup() {
  // initialise servo motors
  servo_disc.setPeriodHertz(50);
	servo_disc.attach(SERVO_PIN_DISC, 500, 2500);
  servo_claw.setPeriodHertz(50);
	servo_claw.attach(SERVO_PIN_CLAW, 500, 2500);
  //servo_disc.write(180);
  displayMessage("this is a test");
}
void loop() {
  displayMessage("this is a test");
}