#include <AccelStepper.h>

AccelStepper azimuthStepper(AccelStepper::FULL4WIRE, 25, 26, 33, 27);

int sensorPin1 = A0; // select the input pin for LDR
int sensorPin2 = A1;
int sensorValue1 = 0; // variable to store the value coming from the sensor
int sensorValue2 = 0;

void setup() {
  Serial.begin(9600); //sets serial port for communication
  azimuthStepper.setMaxSpeed(100);
  azimuthStepper.setAcceleration(20);
  azimuthStepper.moveTo(500);
}

void rotateRight(int currPos, AccelStepper motor){
  new_pos = curr_pos * 1.1
  motor.moveTo(new_pos)
}

void rotateLeft(int currPos){
  new_pos = curr_pos * -1.1
  motor.moveTo(new_pos)
}

int calculatePanelAngle(int left_sensor, int right_sensor){

  sensor_difference = abs(left_sensor - right_sensor)
  sensor_buffer = 110

  if (sensor_difference > sensor_buffer) {

    if (left_sensor < right_sensor){
      // Tilt panel left

      // Code to move the panel position so that the new light readings are within the buffer
      while (sensor_difference > sensor_buffer){

        // move motor left some x amount ~ prolly a few degrees
        rotateLeft(azimuthStepper.currentPosition(), azimuthStepper)

        right_sensor = analogRead(sensorPin1);
        left_sensor = analogRead(sensorPin2);
        sensor_difference = abs(left_sensor - right_sensor);
      }
    }

    else {
      while (sensor_difference > sensor_buffer){

        // move motor right some x amount ~ prolly a few degrees
        rotateRight(azimuthStepper.currentPosition(), azimuthStepper)

        right_sensor = analogRead(sensorPin1);
        left_sensor = analogRead(sensorPin2);
        sensor_difference = abs(left_sensor - right_sensor);
      }
    }
  }
}

void loop() {
  sensorValue1 = analogRead(sensorPin1); // read the value from the sensor
  sensorValue2 = analogRead(sensorPin2);

  
  Serial.print("Pin 1: ");
  Serial.print(sensorValue1);
  Serial.print(", Pin 2: ");
  Serial.println(sensorValue2);
  delay(1000);
}