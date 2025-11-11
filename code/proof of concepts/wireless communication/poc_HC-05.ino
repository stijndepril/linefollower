#include "Arduino.h"
#include <SoftwareSerial.h>
 
const byte rxPin = 8;
const byte txPin = 9;
int interruptPin = 2;
char DataFromRobot;
char DataFromPhone;
bool state = LOW;
SoftwareSerial BTSerial(rxPin, txPin); // RX TX
 
void setup() {
// define pin modes for tx, rx:
pinMode(rxPin, INPUT);
pinMode(txPin, OUTPUT);
pinMode(LED_BUILTIN, OUTPUT);
pinMode(interruptPin, INPUT);
attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt, RISING);
BTSerial.begin(9600);
Serial.begin(9600);
}
 
void loop() {

//data die ontvangen wordt vanuit de bluetoothmodule
if (BTSerial.available() > 0) {
  DataFromPhone = BTSerial.read();
  Serial.write(DataFromPhone); //stuurt data van gsm naar seriele monitor
}
if (Serial.available() > 0 ) {
  DataFromRobot = Serial.read();
  BTSerial.write(DataFromRobot); //stuurt data van seriele monitor naar gsm
}
  
  // zet de built-in LED aan als deze wordt bediend door de knop op de gsm
  if (DataFromPhone == '0') {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (DataFromPhone == '1') {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void interrupt() {
 static unsigned long vorigeInterrupt = 0;
 unsigned long interruptTijd = millis();
 if (interruptTijd - vorigeInterrupt > 200){
 BTSerial.write("De knop is ingedrukt!"); // stuurt door als de knop wordt ingedrukt naar de gsm
 }
 vorigeInterrupt = interruptTijd;
}
