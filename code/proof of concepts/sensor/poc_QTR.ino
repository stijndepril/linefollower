#include <QTRSensors.h>

QTRSensors qtr;

const uint8_t SensorCount = 6;
uint16_t sensorValues[SensorCount];

void setup()
{
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5}, SensorCount);
  qtr.setEmitterPin(2);

  Serial.begin(9600);
}


void loop()
{
  // Lezen sensorwaardes
  qtr.read(sensorValues);

  // Schrijven van sensorwaardes (0 tot 1023), met 0 = max reflectie en 1023 = min reflectie
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();

  delay(250);
}
