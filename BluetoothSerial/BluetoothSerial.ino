#include "BluetoothSerial.h"
#include <AccelStepper.h>

#define HALFSTEP 8

// Define the steppers and the pins it will use
AccelStepper azimuthStepper(HALFSTEP, 25, 26, 33, 27);
AccelStepper betaStepper(HALFSTEP,19, 22, 23, 18);

// Checks if Bluetooth is properly enabled.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Creates an instance of BluetoothSerial called SerialBT
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);         // Initialize a serial communication at a baud rate of 115200
  SerialBT.begin("SYDEGroup16");   // Bluetooth device name SYDEGroup16
  Serial.println("The device started, now you can pair it with bluetooth!");

  //Setup defaults for both steppers 
  betaStepper.setMaxSpeed(100);
  betaStepper.setAcceleration(20);

  azimuthStepper.setMaxSpeed(100);
  azimuthStepper.setAcceleration(20);
}

/* 
  Within the loop, we're listening and reading data from our computer using Bluetooth Serial 
  connection. From the computer, we send strings as bytes to move our motors. Currently, we've
  defined 3 cases of input

  1. 'A,50' (A identifies the azimuth motor, 50 is the angle to move [absolute])
  2. 'B,80' (B identifies the beta motor, 80 is the angle to move [absolute])
  3. 'I,30,20' (I identifies the increment, 50 is optimal adjustment angle, 20 is adjustment time[mins] )

*/

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readString();
    //Process the input string 
    String id = input.substring(0,1);
    String value = input.substring(2);
    value.trim();

    if(id == String('A')){
      double steps = value.toInt() * 5.69 * 2;
      azimuthStepper.moveTo(steps);

      Serial.println(value.toInt()); 
      Serial.println();
    } else if (id == String('B')){ 
      double steps = value.toInt() * 5.69 * 2;
      betaStepper.moveTo(steps);

      Serial.println(value.toInt()); 
      Serial.println();
    } else if (id == String('I')) {
      int adjustmentAngle = value.substring(0,value.indexOf(',')).toInt();
      int adjustmentTimeMins = value.substring(value.indexOf(',') + 1).toInt();

      Serial.println(adjustmentAngle); 
      Serial.println(adjustmentTimeMins); 
      Serial.println();
    }
  }
  
  // Set the speed and run the steppers (these should be called as often as possible)
  azimuthStepper.setSpeed(100);
  azimuthStepper.runSpeedToPosition();
  betaStepper.setSpeed(100);
  betaStepper.runSpeedToPosition();

}
