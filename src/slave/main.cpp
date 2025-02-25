#include <Arduino.h>
#include <Wire.h>

const uint8_t SLAVE_ADDRESS = 42;
const uint8_t LED_PIN = LED_BUILTIN;

uint8_t ledVal = 0;
uint16_t twiInitJitter;
bool twiInitialized = false;
unsigned long previousTime = 0;

void receiveEvent(int howMany);

void setup()
{
  Serial.begin(9600);

  randomSeed(analogRead(A1));
  twiInitJitter = random(100000);

  Wire.onReceive(receiveEvent);
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  if (micros() - previousTime >= twiInitJitter && !twiInitialized)
  {
    Wire.begin(SLAVE_ADDRESS);

    // Enable broadcast reception
    TWAR = (SLAVE_ADDRESS << 1) | 1;

    twiInitialized = true;
    previousTime = micros();
  }
}

void receiveEvent(int howMany)
{
  // Expecting 2 bytes; verify and process
  if (howMany == 2)
  {
    uint16_t value = Wire.read() << 8 | Wire.read();
    Serial.print("Received value: ");
    Serial.println(value);

    digitalWrite(LED_PIN, ledVal ^= 1); // Toggle LED for visual confirmation
  }

  // Flush any extra data
  while (Wire.available() > 0)
  {
    Wire.read();
  }
}