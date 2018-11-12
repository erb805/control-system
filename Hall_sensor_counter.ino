/* Erik Bruenner
 * Bryce Campbell
 * Monty Choy
 * 
 * 11/3/18
 *
 * Last Updated - 11/9/18
 *
 * Code for Hall sensor Counter
 */

#include <SoftwareSerial.h>
#include <Wire.h>

//adjust this delay as needed (hopefully none once we hood it up and test)
#define COUNTER_DELAY 100 // wait this amount of time after each interupt

//defines the slave address of the uno
#define SLAVE_ADDRESS 0x04

//sets the counter Sensor pin
const int counterSensorPin = 2; 

//String array denoting requestType, could add laserCount, etc...
const String requestType[] = {"HALL_COUNT"};

//keeps track of request type so different data can be sent in the future
volatile String request;

//timer to make sure we dont get too many signals
volatile double counterTime = millis(); // timer to prevent the interrupt from being called mulltiple times per strip.

//keeps track of the total number of signals encountered
volatile int totalTicks = 0; // the number of ticks the pod has traveled

/*
 * Main loop
 */
 
void loop() {
  Serial.println(totalTicks); // print the total ticks for debug
  delay(1000);
}

void setup() {
  Serial.begin(9600); // start serial for output

  //initialize counter interupt and I2C stuff
  counter_setup();

  //setup I2C stuff
  I2C_setup();
}

/*
 * Interrupt service routine for the counter
 *
 *  Waits for laserDelay amount of milliseconds and then increments the ticksSinceLastCheck when the counter detects a signal
 */
void counter_isr() {
  if(counterTime + COUNTER_DELAY < millis()) // pause for laserDelay number of millis after interrupt is triggered
  {
    Serial.println("Interrupt");
    totalTicks++; // increment number of ticks since the last check
    counterTime = millis();
  }
}

/*
 * Sets up the interrupt for the counter
 */
void counter_setup()
{
  pinMode(counterSensorPin, INPUT_PULLUP);
  // TODO: determine if this should be FALLING or LOW
  attachInterrupt(digitalPinToInterrupt(counterSensorPin), counter_isr, FALLING); // trigger interrupt when signal from laser falls to 0
}

//initializes this arduino as an I2C slave, establishes callbacks
void I2C_setup()
{
    // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);
  
  // define callbacks for i2c communication
    Wire.onRequest(sendData);
    Wire.onReceive(receiveData);
}

//returns the number of ticks encountered since reset
int get_hall_count()
{
  return totalTicks;
}

void set_hall_count(int i)
{
  totalTicks = i;
}
//
//resets count when an I2C signal is encountered by 
void resetCount()
{
  set_hall_count(0);
}

// callback for sending data
void sendData(){
    if (request == "HALL_COUNT")
    {
      Wire.write(get_hall_count());
      resetCount();
    }
}

void receiveData(int byteCount)
{
  while (Wire.available())
  {
     request = requestType[Wire.read()];
  }
}
