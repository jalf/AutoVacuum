#include "ACS712.h"

const int switch_pin = 5;
const int relay_pin = 2;
const int period = 5000;

bool powerConnected = false;
bool shutingDown = false;
unsigned long start_time = 0;

ACS712  ACS(A0, 5.0, 1023, 66);

void setup() 
{
  pinMode(relay_pin, OUTPUT);
  pinMode(switch_pin, INPUT);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  // Initialize the current sensor
  ACS.autoMidPoint();
}

void loop() 
{
  // Read the current input
  int mA = ACS.mA_AC();
  // Read the switch 
  int switch_status = digitalRead(switch_pin);
  
  // Check if we need turn on the vacuum
  if (!powerConnected && (mA>200 || switch_status == HIGH))
  {
    powerConnected = true;
    shutingDown = false;
    digitalWrite(relay_pin, HIGH);
    Serial.println("ON");
  }

  // Check if we need sign to turn off the vacuum
  if (switch_status == LOW && powerConnected && mA < 100)
  {
      powerConnected = false;
      shutingDown = true;
      start_time = millis();
      Serial.println("Turning off...");
  }

  // Delay 5 seconds to turn off the vacuum
  if(shutingDown && (millis() - start_time > period)){
      digitalWrite(relay_pin, LOW);
      shutingDown = false;
      Serial.println("OFF");
  }
}
