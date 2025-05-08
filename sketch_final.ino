#include <Servo.h>
#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define irEntry_pin 2
#define irInside_pin 3
#define servoPin 6

Servo gateServo;
const short int totAvail = 3;
short int entryCount = 0, currAvail = 3;
short int greenLED = 5, redLED = 4;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// IR receivers
IRrecv entrySensor(irEntry_pin);
IRrecv insideSensor(irInside_pin);
decode_results results;

void setup() {
  entrySensor.enableIRIn();
  insideSensor.enableIRIn();

  lcd.init();
  lcd.backlight();

  pinMode(irEntry_pin, INPUT);
  pinMode(irInside_pin, INPUT);                
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  gateServo.attach(servoPin);
  gateServo.write(0); // Gate initially closed

  lcd.setCursor(2, 0);
  lcd.print("Parking Open");
  lcd.setCursor(1, 1);
  lcd.print("Slots left : ");
  lcd.setCursor(14, 1);
  lcd.print(currAvail);
}

void loop() {
  if (entrySensor.decode(&results)) {
    entrySensor.resume();
    entryfn();
  }

  if (insideSensor.decode(&results)) {
    insideSensor.resume();
    exitfn();
  }
}

void updateAvailable() {
  currAvail = totAvail - entryCount;
}

void ledout() {
  if (currAvail > 0) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
  }
}

void entryfn() {
  if (currAvail <= 0) return;

  insideSensor.disableIRIn();
  gateServo.write(90);  // Open gate
  delay(2500);
  gateServo.write(0);   // Close gate
  entryCount++;
  updateAvailable();
  ledout();
  updateDisplay();
  insideSensor.enableIRIn();
}

void exitfn() {
  if (entryCount <= 0) return;

  entrySensor.disableIRIn();
  gateServo.write(90);
  delay(2500);
  gateServo.write(0);
  entryCount--;
  updateAvailable();
  ledout();
  updateDisplay();
  entrySensor.enableIRIn();
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.clear();

  if (currAvail > 0) {
    lcd.setCursor(2, 0);
    lcd.print("Parking Open");
    lcd.setCursor(1, 1);
    lcd.print("Slots left : ");
    lcd.setCursor(14, 1);
    lcd.print(currAvail);
  } else {
    lcd.setCursor(2, 0);
    lcd.print("Parking Closed");
    lcd.setCursor(1, 1);
    lcd.print("Slots left : 0");
  }
}
