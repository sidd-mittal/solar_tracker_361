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

// Motor Variables 
double azimuthSteps = NULL; 
double betaSteps = NULL; 
double incrementAngle = NULL;
double incrementTime = NULL; 


bool cong = false; 

void BT_EventHandler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_START_EVT) {
    Serial.println("Initialized SPP");
  }
  else if (event == ESP_SPP_SRV_OPEN_EVT ) {
    Serial.println("Client connected");
  }
  else if (event == ESP_SPP_CLOSE_EVT  ) {
    Serial.println("Client disconnected");
  }
  else if (event == ESP_SPP_CONG_EVT){
    cong = true;
    Serial.println("Client not listening");
  }
}

void setup() {
  Serial.begin(115200);         // Initialize a serial communication at a baud rate of 115200
  SerialBT.begin("SYDEGroup16");   // Bluetooth device name SYDEGroup16
  Serial.println("The device started, now you can pair it with bluetooth!");

  // Setup defaults for both steppers 
  betaStepper.setMaxSpeed(100);
  betaStepper.setAcceleration(20);

  azimuthStepper.setMaxSpeed(100);
  azimuthStepper.setAcceleration(20);

   // Register the callbacks defined above (most important: congestion)
  SerialBT.register_callback(BT_EventHandler);
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
  // If our variables aren't populated, continue to read from the Bluetooth Serial connection
  if (SerialBT.available() && 
      (azimuthSteps == NULL || 
      betaSteps == NULL ||
      incrementAngle == NULL ||
      incrementTime == NULL)
    ){
    String input = SerialBT.readString();
    //Process the input string 
    String id = input.substring(0,1);
    String value = input.substring(2);
    value.trim();

    if(id == String('A')){
      azimuthSteps = value.toInt() * 5.69 * 2;
      azimuthStepper.moveTo(azimuthSteps);
      Serial.print("Azimuth angle set to: ");
      Serial.print(value.toInt()); 
      Serial.println(" degrees");
    } else if (id == String('B')){ 
      if(value.toInt() <= 90){ //Our Beta Stepper should not exceed 90 degrees or go below 0
        betaSteps = value.toInt() * 5.69 * 2;
        betaStepper.moveTo(betaSteps);
        Serial.print("Beta angle set to: ");
        Serial.print(value.toInt()); 
        Serial.println(" degrees");
      }
    } else if (id == String('I')) {
      incrementAngle = value.substring(0,value.indexOf(',')).toInt();
      incrementTime = value.substring(value.indexOf(',') + 1).toInt();
      Serial.print("Increment angle set to: ");
      Serial.print(incrementAngle); 
      Serial.println(" degrees");
      Serial.print("Increment time set to: ");
      Serial.print(incrementTime); 
      Serial.println(" minutes");
    }
  }

  // This is executed after we set the initial position
  if (incrementAngle != NULL && incrementTime != NULL && azimuthStepper.distanceToGo() == 0){ 
    Serial.print("Starting sequence, will move motor by ");
    Serial.print(incrementAngle);
    Serial.print(" degrees in ");
    Serial.print(incrementTime);
    Serial.println(" minutes");
    long incrementTimeMS = incrementTime * 60000;
    delay(incrementTimeMS);
    Serial.println("Moving motor");
    // Here we should check if the increment angle is within bounds 0 < x < 90 before incrementing
    // Serial.println(azimuthStepper.currentPosition()/(5.69 * 2));
    azimuthStepper.move(incrementAngle*5.69*2); //move relatively
    azimuthStepper.setSpeed(100);
    azimuthStepper.runSpeedToPosition();
  }
    // Set the speed and run the steppers (these should be called as often as possible)
  azimuthStepper.setSpeed(100);
  azimuthStepper.runSpeedToPosition();
  betaStepper.setSpeed(100);
  betaStepper.runSpeedToPosition();

}
