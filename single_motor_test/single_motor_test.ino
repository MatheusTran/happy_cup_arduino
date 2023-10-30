// libraries
#include <Wire.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>


// defining pins
#define SERVO_PIN_DISC       13   
#define SERVO_PIN_CLAW       14

#define BUTTON_PIN_RED       25 //red
#define BUTTON_PIN_YELLOW    33 // yellow

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
  //make input pins
  pinMode(BUTTON_PIN_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RED, INPUT_PULLUP);

    // Initialize the display
  u8g2.begin();

  // Set font and size
  u8g2.setFont(u8g2_font_ncenB14_tr);


  // initialise servo motors
  servo_disc.setPeriodHertz(50);
	servo_disc.attach(SERVO_PIN_DISC, 500, 2500);
  servo_claw.setPeriodHertz(50);
	servo_claw.attach(SERVO_PIN_CLAW, 500, 2500);
  servo_claw.write(40);
  //servo_disc.write(180);
  displayMessage("this is a test");
}
void loop() {
  if (digitalRead(BUTTON_PIN_YELLOW) == LOW) {  
    displayMessage("claw motor");
    // servo_disc.write(90);
    servo_claw.write(38);
    delay(300);
    servo_claw.write(28);
  } else if (digitalRead(BUTTON_PIN_RED) == LOW || isRedPressed) {
    displayMessage("disc motor");
    servo_disc.write(130);
    displayMessage("waiting 2 seconds");
    delay(2000);
    servo_disc.write(90);
  } else {
    displayMessage("Press a button");
  }
  delay(100);
}