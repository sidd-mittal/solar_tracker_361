#include "BluetoothSerial.h"
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

/******************
  SETUP FOR MOTORS
*******************/
#define HALFSTEP 8

// Initialize variables used for stepper motors
AccelStepper azimuthStepper(HALFSTEP, 25, 26, 33, 27);
AccelStepper betaStepper(HALFSTEP,19, 4, 23, 18); 
 
double azimuthSteps = NULL; 
double betaSteps = NULL; 
double incrementAngle = NULL;
double incrementTime = NULL; 

/*****************
  SETUP FOR INA219
******************/
Adafruit_INA219 ina219;
int base = 32; // Pin connected to base pin on the transistor (used for monitoring current)
// Reporting frequency
float freq = 1/2; // Hz
// Delay after changing state of transistor
int del = 2; 

// Sensor variables 
float current_mA = 0;
float voltage = 0;
float power_mW = 0;

// Tracking time
unsigned long last1 = 0;
unsigned long last2 = 0;
unsigned long led_last = 0;
float t = 0;
int led = 2000;
bool state = false;

/*********************
  SETUP FOR BLUETOOTH
**********************/
BluetoothSerial SerialBT;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

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
  Serial.begin(115200);    // Initialize a serial communication at a baud rate of 115200

  SerialBT.begin("SYDEGroup16");   // Bluetooth device name SYDEGroup16
  Serial.println("The device started, now you can pair it with bluetooth!");
  SerialBT.register_callback(BT_EventHandler); // Register the callbacks defined above (most important: congestion)

  // Setup defaults for both steppers 
  //Start beta horizontal
  betaStepper.setMaxSpeed(50);
  betaStepper.setAcceleration(100000);

  //start oriented east 
  azimuthStepper.setMaxSpeed(50);
  azimuthStepper.setAcceleration(20);


  //Setup for INA219
  pinMode(base, OUTPUT);
  pinMode(2, OUTPUT);
  ina219.begin();
  ina219.setCalibration_16V_400mA();

  Serial.println("Measuring voltage and current with INA219 ...");

  last1 = millis();
  last2 = millis();
  led_last = millis();
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

  /************
  Monitoring Current
  *************/
  if (millis() - last1 >= 60000 ){
    last1 = millis();
    t = last1/1000.0; // Time in seconds


    // MODIFY THIS TO OBTAIN Voc and Isc
    // Change the voltage on the transistor BASE as needed using:

    //HIGH = Short circut (Isc)
    digitalWrite(base, HIGH);
    delay(del);
    current_mA = ina219.getCurrent_mA();
    delay(del);

    //LOW = Open circut (Voc)
    digitalWrite(base, LOW);
    delay(del);
    voltage = ina219.getBusVoltage_V();
    
    // Max power (mW) is at roughly 0.7x the voltage
    power_mW = (0.7*voltage)*current_mA;
    
    // Format: Time, Voltage, Current, Estimated Power
    Serial.print(t); Serial.print(", "); 
    Serial.print(voltage); Serial.print(", "); 
    Serial.print(current_mA); Serial.print(", ");
    Serial.println(power_mW);
  }

  // Blink the blue LED while we're acquiring data
  if (millis() - led_last > led){
    state = !state;
    if (state){digitalWrite(2,HIGH);
    }else{digitalWrite(2,LOW);
    }
    led_last = millis();
  }

  /************
  Bluetooth
  *************/
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
      azimuthSteps = value.toFloat() * 5.69 * 2;
      azimuthStepper.moveTo(azimuthSteps);
      Serial.print("Azimuth angle set to: ");
      Serial.print(value.toFloat()); 
      Serial.println(" degrees");
    } else if (id == String('B')){ 
      if(value.toFloat() <= 90){ //Our Beta Stepper should not exceed 90 degrees or go below 0
        betaSteps = value.toFloat() * 5.69 * 2;
        betaStepper.moveTo(betaSteps);
        Serial.print("Beta angle set to: ");
        Serial.print(value.toFloat()); 
        Serial.println(" degrees");
      }
    } else if (id == String('I')) {
      incrementAngle = value.substring(0,value.indexOf(',')).toFloat();
      incrementTime = value.substring(value.indexOf(',') + 1).toFloat();
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
    long incrementTimeMS = incrementTime * 60000;
    if (millis() - last2 >= incrementTimeMS){
      last2 = millis();
      Serial.print("Starting sequence, will move motor by ");
      Serial.print(incrementAngle);
      Serial.print(" degrees in ");
      Serial.print(incrementTime);
      Serial.println(" minutes");
      Serial.println("Moving motor");
      // Here we should check if the increment angle is within bounds 0 < x < 90 before incrementing
      azimuthStepper.move(incrementAngle*5.69*2); //move relatively
      azimuthStepper.setSpeed(100);
      azimuthStepper.runSpeedToPosition();
    }
  }
    // Set the speed and run the steppers (these should be called as often as possible)
  azimuthStepper.setSpeed(100);
  azimuthStepper.runSpeedToPosition();
  betaStepper.setSpeed(50);
  betaStepper.runSpeedToPosition();

}
