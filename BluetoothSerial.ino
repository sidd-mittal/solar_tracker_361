#include "BluetoothSerial.h" //library

//checks if Bluetooth is properly enabled.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//creates an instance of BluetoothSerial called SerialBT
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);         //initialize a serial communication at a baud rate of 115200.
  SerialBT.begin("CDBtest");   //Bluetooth device name CDBtest is the argument name, can be changed to anything unique
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read()); 
    //SerialBT.write() sends data using bluetooth serial.
    //Serial.read() returns the data received in the serial port.
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read()); 
  }
  delay(20);
}