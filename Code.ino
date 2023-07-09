// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::FULL4WIRE,19,22,23,18);
#include "BluetoothSerial.h" //library

//checks if Bluetooth is properly enabled.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif

//creates an instance of BluetoothSerial called SerialBT
BluetoothSerial SerialBT;


// assuming motor power usage of 1.5W so opimtal adjustment period of 41 mins
int optimal_adjustment_mins = 41;
int number_of_daylight_hours = 15;
int adjustment_amount = 1024/((number_of_daylight_hours*60)/optimal_adjustment_mins);

void setup()
{  
  Serial.println("Hello");
  Serial.begin(115200);         //initialize a serial communication at a baud rate of 115200.
  SerialBT.begin("CDBtest");  
  Serial.println("The device started, now you can pair it with bluetooth!");
   

}

void loop()
{  
    if (Serial.available()) {
      SerialBT.write(Serial.read()); 
      //SerialBT.write() sends data using bluetooth serial.
      //Serial.read() returns the data received in the serial port.
    }
    if (SerialBT.available()) {
      Serial.write(SerialBT.read()); 
      int optimal_B = (Serial.read());
      
    }
    delay(20);

    
    stepper.runSpeed();
    stepper.moveTo(adjustment_amount); //rotates from 0 to 1024 over time 
    delay(optimal_adjustment_mins * 60000);

   
   
}
