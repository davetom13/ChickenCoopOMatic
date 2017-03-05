#include <LiquidCrystal.h>
#include <Adafruit_MotorShield.h>
#include "doormanager.h"
#include "lightsensor.h"
#include "latchingswitch.h"

//pins
const int topSwitchPin = 3;
const int bottomSwitchPin = 2;

const int lightPin = 4;

const int stepsPerRotation = 200; //5373;
const int openLevel = 700;
const int closeLevel = 550;

Adafruit_MotorShield afms = Adafruit_MotorShield(); 
Adafruit_StepperMotor* motor = afms.getStepper(stepsPerRotation, 2);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

LatchingSwitch* topSwitch;
LatchingSwitch* bottomSwitch;
LightSensor* sensor;
Display* display;

DoorManager* door;

void setup() {
  Serial.begin(9600);
  pinMode(topSwitchPin, INPUT);

  lcd.begin(16, 2);               // start the library

  display = new Display(&lcd);

  display->printStatus("Starting up", "Chook-O-Matic", false);
  
  afms.begin();  // create with the default frequency 1.6KHz
  motor->setSpeed(60);  // 10 rpm   

  topSwitch = new LatchingSwitch(topSwitchPin, "Top");
  bottomSwitch = new LatchingSwitch(bottomSwitchPin, "Bottom");
  sensor = new LightSensor(lightPin);

  topSwitch->reset();
  bottomSwitch->reset();
  
  door = new DoorManager(topSwitch, bottomSwitch, sensor, display, motor);

  door->setLightLevel(UP, openLevel);
  door->setLightLevel(DOWN, closeLevel);
  
  door->start();
  Serial.println("Setup complete");
}

void loop() {
  door->work();
}

