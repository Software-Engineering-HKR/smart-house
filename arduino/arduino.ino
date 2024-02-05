#include <Servo.h>

Servo windowServo;
Servo doorServo;   

// Pin assignments
const int lightSensorPin = A1; 
const int gasSensorPin = A0; 
const int ledPin = 13;     
const int fanPin1 = 7;  
const int fanPin2 = 6;       
const int windowPin = 10;  
const int doorPin = 9;      

// State tracking variables
bool isWindowOpen = false;
bool isDoorOpen = false;
bool isFanOn = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin1, OUTPUT);
  pinMode(fanPin2, OUTPUT);
  windowServo.attach(windowPin);  
  doorServo.attach(doorPin);      
  Serial.begin(9600);
}

void loop() {
  sendSensorDataAtInterval();
  handleSerialCommands();
}

void sendSensorDataAtInterval() {
  static unsigned long lastSensorSendTime = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorSendTime >= 10000) {
    sendSensorDataAsJson();
    lastSensorSendTime = currentMillis;
  }
}

void sendSensorDataAsJson() {
  int lightSensorValue = analogRead(lightSensorPin);
  int gasSensorValue = analogRead(gasSensorPin);

  Serial.print("{\"light\":");
  Serial.print(lightSensorValue);
  Serial.print(",\"gas\":");
  Serial.print(gasSensorValue);
  Serial.print(",\"led\":");
  Serial.print(digitalRead(ledPin) ? "true" : "false");
  Serial.print(",\"fan\":");
  Serial.print(isFanOn ? "true" : "false");
  Serial.print(",\"window\":");
  Serial.print(isWindowOpen ? "true" : "false");
  Serial.print(",\"door\":");
  Serial.print(isDoorOpen ? "true" : "false");
  Serial.println("}");
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
  doorServo.write(90);
  isDoorOpen = true;
}

void closeDoor() {
  doorServo.write(0);
  isDoorOpen = true;
}
