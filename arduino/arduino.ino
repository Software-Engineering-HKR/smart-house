#include <Servo.h>

Servo windowServo;
Servo doorServo;   

// Pin assignments
const int lightSensorPin = A5; 
const int gasSensorPin = A4; 
const int ledPin = 13;     
const int yellowLedPin = 5;
const int fanPin1 = 7;  
const int fanPin2 = 6;       
const int windowPin = 10;  
const int doorPin = 9;      
const int motionSensorPin = 2;
const int steamSensorPin = 3;
const int moistureSensorPin = A3;


// State tracking variables
bool isWindowOpen = false;
bool isDoorOpen = false;
bool isFanOn = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin1, OUTPUT);
  pinMode(fanPin2, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(motionSensorPin, INPUT); 
  pinMode(steamSensorPin, INPUT);
  pinMode(moistureSensorPin, INPUT);
  windowServo.attach(windowPin);  
  doorServo.attach(doorPin);      
  Serial.begin(9600);
}

void loop() {
  handleMotionSensor();
  sendSensorDataAtInterval();
  handleSerialCommands();
}

void handleMotionSensor() {
  int motionDetected = digitalRead(motionSensorPin);
  if (motionDetected) {
    // Perform actions when motion is detected
    // For example, turn on LED or open door
    // digitalWrite(ledPin, HIGH);
        // digitalWrite(yellowLedPin, HIGH);
  } else {
    // digitalWrite(ledPin, LOW);
        // digitalWrite(yellowLedPin, LOW);

  }
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
