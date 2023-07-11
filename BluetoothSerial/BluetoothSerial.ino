#include "BluetoothSerial.h" //library
#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper betaStepper(AccelStepper::FULL4WIRE,19, 22, 23, 18); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper azimuthStepper(AccelStepper::FULL4WIRE, 25, 26, 33, 27); 

//checks if Bluetooth is properly enabled.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//creates an instance of BluetoothSerial called SerialBT
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);         //initialize a serial communication at a baud rate of 115200.
  SerialBT.begin("CDB-Robichaud");   //Bluetooth device name CDBtest is the argument name, can be changed to anything unique
  Serial.println("The device sstarted, now you can pair it with bluetooth!");

  betaStepper.setMaxSpeed(100);
  betaStepper.setAcceleration(20);

  azimuthStepper.setMaxSpeed(100);
  azimuthStepper.setAcceleration(20);
  azimuthStepper.moveTo(500);
}

void loop() {
  if (Serial.available()) {
    // SerialBT.write() sends data using bluetooth serial.
    // Serial.read() returns the data received in the serial port.
  }

  if (SerialBT.available()) {
    int input = SerialBT.readString().toInt();
    double steps = input * 5.69;
    betaStepper.moveTo(steps);
  }

  betaStepper.setSpeed(100);
  betaStepper.runSpeedToPosition();

  if (azimuthStepper.distanceToGo() == 0)
      azimuthStepper.moveTo(-azimuthStepper.currentPosition());

    azimuthStepper.run();
}
