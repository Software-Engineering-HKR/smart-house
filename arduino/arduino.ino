#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd (0x27,16,2);

Servo windowServo;
Servo doorServo;   

// Pin assignments
const int gasSensorPin = A0;
const int lightSensorPin = A1;
const int moistureSensorPin = A2;
const int steamSensorPin = A3;
const int motionSensorPin = 2;
int tonepin = 3;
int leftBtnpin = 4;
const int yellowLedPin = 5;
const int fanPin2 = 6;     
const int fanPin1 = 7;
int rightBtnpin = 8; 
const int doorPin = 9;   
const int windowPin = 10;
const int ledPin = 13;



// State tracking variables
bool isWindowOpen = false;
bool isDoorOpen = false;
bool isFanOn = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin1, OUTPUT);
  pinMode(fanPin2, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode (tonepin, OUTPUT);
  pinMode(motionSensorPin, INPUT); 
  pinMode(steamSensorPin, INPUT);
  pinMode(moistureSensorPin, INPUT);
  pinMode (leftBtnpin, INPUT);
  pinMode (rightBtnpin, INPUT);
  windowServo.attach(windowPin);  
  doorServo.attach(doorPin);    
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop() {
  handleDoorbell();
  handleOpenBtn();
  handleMotionSensor();
  sendSensorDataAtInterval();
  handleSerialCommands();
}


void handleDoorbell() {
  int val = digitalRead(leftBtnpin); 

  while (!val) {
    // Perform actions when motion is detected
    for (unsigned char i = 0; i < 80; i++) { // Output a frequency sound
      digitalWrite(tonepin, HIGH); // Sound
      delay(1); // Delay 1ms
      digitalWrite(tonepin, LOW); // No sound
      delay(1); // Delay 1ms
    }
    for (unsigned char i = 0; i < 100; i++) { //
      digitalWrite(tonepin, HIGH); // 
      delay(2); 
      digitalWrite(tonepin, LOW);
      delay(2);
    }
    val = digitalRead(leftBtnpin); 
  }
}

void handleOpenBtn() {
  static bool lastButtonState = HIGH;
  int val = digitalRead(rightBtnpin);

  if (val == LOW && lastButtonState == HIGH) {
    if (!isDoorOpen) {
      openDoor();
      isDoorOpen = true; 
    } else {
      closeDoor();
      isDoorOpen = false; 
    }
  }
  
  lastButtonState = val; // Update the last button state
}

void handleMotionSensor() {
  int motionDetected = digitalRead(motionSensorPin);
  // while (motionDetected) { // Loop while motion is detected
  //   // Perform actions when motion is detected
  //   for (unsigned char i = 0; i < 80; i++) { // Output a frequency sound
  //     digitalWrite(tonepin, HIGH); // Sound
  //     delay(1); // Delay 1ms
  //     digitalWrite(tonepin, LOW); // No sound
  //     delay(1); // Delay 1ms
  //   }
  //   for (unsigned char i = 0; i < 100; i++) { // Output sound of another frequency
  //     digitalWrite(tonepin, HIGH); // Sound
  //     delay(2); // Delay 2ms
  //     digitalWrite(tonepin, LOW); // No sound
  //     delay(2); // Delay 2ms
  //   }
  //   motionDetected = digitalRead(motionSensorPin); // Re-check motion
  // }
}

void sendSensorDataAtInterval() {
  static unsigned long lastSensorSendTime = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorSendTime >= 1000) {
    sendSensorDataAsJson();
    lastSensorSendTime = currentMillis;
  }
}

void sendSensorDataAsJson() {
  int lightSensorValue = analogRead(lightSensorPin);
  int gasSensorValue = analogRead(gasSensorPin);
  int motionDetected = digitalRead(motionSensorPin);
  int steamSensorValue = analogRead(steamSensorPin);
  int moistureSensorValue = analogRead(moistureSensorPin);

  // Construct the JSON string
  String json = "{\"sensors\":{\"light\":" + String(lightSensorValue) +
                ",\"gas\":" + String(gasSensorValue) +
                ",\"motion\":" + String(motionDetected) +
                ",\"steam\":" + String(steamSensorValue) +
                ",\"moisture\":" + String(moistureSensorValue) +
                "},\"devices\":{\"led\":" + (digitalRead(ledPin) ? "true" : "false") +
                ",\"yellow-led\":" + (digitalRead(yellowLedPin) ? "true" : "false") +
                ",\"fan\":" + (isFanOn ? "true" : "false") +
                ",\"window\":" + (isWindowOpen ? "true" : "false") +
                ",\"door\":" + (isDoorOpen ? "true" : "false") + "}}";

  // Send the JSON string
  Serial.println(json);
}


void handleSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    executeCommand(command);
  }
}

void executeCommand(String command) {
  if (command == "LED_ON") {
    digitalWrite(ledPin, HIGH);
  } else if (command == "LED_OFF") {
    digitalWrite(ledPin, LOW);
  } else if (command == "YELLOWLED_ON") {
    digitalWrite(yellowLedPin, HIGH);
  } else if (command == "YELLOWLED_OFF") {
    digitalWrite(yellowLedPin, LOW);
  } else if (command == "FAN_ON") {
    fanOn();
  } else if (command == "FAN_OFF") {
    fanOff();
  } else if (command == "WINDOW_OPEN") {
    openWindow();
  } else if (command == "WINDOW_CLOSE") {
    closeWindow();
  } else if (command == "DOOR_OPEN") {
    openDoor();
  } else if (command == "DOOR_CLOSE") {
    closeDoor();
  } else if (command.startsWith("LCD/")) {
    String message = command.substring(4);
    setLCD(message);
  }
}

void fanOn() {
  digitalWrite(fanPin1, LOW);
  digitalWrite(fanPin2, HIGH);
  isFanOn = true;
}

void fanOff() {
  digitalWrite(fanPin1, LOW);
  digitalWrite(fanPin2, LOW);
  isFanOn = false;
}

void openWindow() {
  windowServo.write(180);
  isWindowOpen = true;
}

void closeWindow() {
  windowServo.write(0);
  isWindowOpen = false;
}

void openDoor() {
  doorServo.write(0);
  isDoorOpen = true;
}

void closeDoor() {
  doorServo.write(90);
  isDoorOpen = false;
}

void setLCD(String message) {
  lcd.init();
  lcd.print(message);
}
