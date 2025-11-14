#include "Arduino.h"
#include "SerialCommand.h"
#include "EEPROMAnything.h"
#include <SoftwareSerial.h>
 
const byte rxPin = 9;
const byte txPin = 8;
#define Baudrate 9600

#define MotorLeftForward 10
#define MotorLeftBackward 11
#define MotorRightForward 5
#define MotorRightBackward 6

SoftwareSerial BTSerial(rxPin, txPin); // RX TX
SerialCommand sCmd(BTSerial);
bool debug;
bool run;
unsigned long previous, calculationTime;

const int sensor[] = {A0, A1, A2, A3, A4, A5};
struct param_t
{
  unsigned long cycleTime;
  int black[6];
  int white[6];
  int power;
  float diff;
  float kp;
  float ki;
  float kd;
  //andere parameters die in het geheugen moeten
} params;

int normalised[6];
float debugPosition;
float iTerm, lastErr;

void setup()
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  BTSerial.begin(Baudrate);
  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("calibrate", onCalibrate);
  sCmd.addCommand("run", onRun);
  sCmd.addCommand("cmd", onCmd);
  sCmd.setDefaultHandler(onUnknownCommand);

  EEPROM_readAnything(0, params);
  
  BTSerial.println("ready");
}

void loop()
{
 sCmd.readSerial();

 unsigned long current = micros();
 if (current - previous >= params.cycleTime)
 {
  previous = current;
  
  //Waardes normaliseren: 
  for (int i = 0; i < 6; i++) normalised[i] = map(analogRead(sensor[i]), params.black[i], params.white[i], 0, 1000);

  //Kwadratische interpolatie:
  int index = 0;
  float position;
  for (int i=1; i < 6; i++) if (normalised[i] < normalised[index]) index = i;

  if (index == 0) position = -20;
  else if (index == 5) position = 20;
  else
  {
    int sNul = normalised[index];
    int sMinEen = normalised[index-1];
    int sPlusEen = normalised[index+1];

    float b = sPlusEen - sMinEen;
    b = b / 2;
    float a = sPlusEen - b - sNul;
    position = -b /(2 * a);
    position += index;
    position -= 2.5;
    position *= 10;
  }
  debugPosition = position;

  //PID
  float error = -position;
  float output = error * params.kp;
  iTerm += params.ki * error;
  iTerm = constrain(iTerm, -510, 510);
  output += iTerm;
  output += params.kd * (error - lastErr);
  lastErr = error;
  output = constrain(output, -510, 510);
  
  int powerLeft = 0;
  int powerRight = 0;

  if (run) if (output >= 0)
  {
    powerLeft = constrain(params.power + params.diff * output, -255, 255);
    powerRight = constrain(powerLeft - output, -255, 255);
    powerLeft = powerRight + output;
  }
  else
  {
    powerRight = constrain(params.power - params.diff * output, -255, 255);
    powerLeft = constrain(powerRight + output, -255, 255);
    powerRight = powerLeft - output;
  }
  
  analogWrite(MotorLeftForward, powerLeft > 0 ? powerLeft : 0);
  analogWrite(MotorLeftBackward, powerLeft < 0 ? -powerLeft : 0);
  analogWrite(MotorRightForward, powerRight > 0 ? powerRight : 0);
  analogWrite(MotorRightBackward, powerRight < 0 ? -powerRight : 0);
  
 }
 unsigned long difference = micros() - current;
 if (difference > calculationTime) calculationTime = difference;
}

void onSet()
{
  char* parameter = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(parameter, "cycle") == 0)
  {
    long newCycleTime = atol(value);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);
    params.ki *= ratio;
    params.kd /= ratio;
    params.cycleTime = newCycleTime;
  }
  else if (strcmp(parameter, "power") == 0) params.power = atol(value);
  else if (strcmp(parameter, "diff") == 0) params.diff = atof(value);
  else if (strcmp(parameter, "kp") == 0) params.kp = atof(value);
  else if (strcmp(parameter, "ki") ==0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.ki = atof(value) * cycleTimeInSec;
  }
  else if (strcmp(parameter, "kd") ==0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.kd = atof(value) / cycleTimeInSec;
  }
  else BTSerial.println("unknown paramameter");

  EEPROM_writeAnything(0, params);
}

void onDebug()
{
  BTSerial.print("cycle time: ");
  BTSerial.println(params.cycleTime);

  BTSerial.print("black: ");
  for (int i = 0; i < 6; i++)
  {
    BTSerial.print(params.black[i]);
    BTSerial.print(" ");    
  }
  BTSerial.println(" ");
  
  BTSerial.print("white: ");
  for (int i = 0; i < 6; i++)
  {
    BTSerial.print(params.white[i]);
    BTSerial.print(" ");    
  }
  BTSerial.println(" "); 

  BTSerial.print("normalised: ");
  for (int i = 0; i < 6; i++)
  {
    BTSerial.print(normalised[i]);
    BTSerial.print(" ");    
  }
  BTSerial.println(" ");

  BTSerial.print("position: ");
  BTSerial.println(debugPosition);

  BTSerial.print("power: ");
  BTSerial.println(params.power);

  BTSerial.print("diff: ");
  BTSerial.println(params.diff);

  BTSerial.print("Kp: ");
  BTSerial.println(params.kp);

  float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
  float ki = params.ki / cycleTimeInSec;
  BTSerial.print("Ki: ");
  BTSerial.println(ki);
  
  float kd = params.kd * cycleTimeInSec;
  BTSerial.print("Kd: ");
  BTSerial.println(kd);

  BTSerial.print("calculation time: ");
  BTSerial.println(calculationTime);
  calculationTime = 0;
}

void onCalibrate()
{
  char* parameter = sCmd.next();
  if (strcmp(parameter, "black") == 0)
  {
    BTSerial.print("start calibrating black... ");
    for (int i = 0; i < 6; i++) params.black[i] = analogRead(sensor[i]);
    BTSerial.print("done");
  }
  else if (strcmp(parameter, "white") == 0)
  {
    BTSerial.print("start calibrating white... ");
    for (int i = 0; i < 6; i++) params.white[i] = analogRead(sensor[i]);
    BTSerial.print("done");
  }
  EEPROM_writeAnything(0, params);
}

void onRun()
{
  char* parameter = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(parameter, "on") == 0)
  {
   iTerm = 0;
   run = true;
  }
  else run = false;
}

void onCmd()
{
  BTSerial.println("-------Possible commands:-------");
  BTSerial.println("calibrate white");
  BTSerial.println("calibrate black");
  BTSerial.println("set cycle 'x' (<10000 µs");
  BTSerial.println("set power 'x' (0...255)");
  BTSerial.println("set diff 'x' (0...1)");
  BTSerial.println("set kp 'x'");
  BTSerial.println("set ki 'x'");
  BTSerial.println("set kd 'x'");
}

void onUnknownCommand(char* command)
{
  BTSerial.print("Unknown Command: \"");
  BTSerial.print(command);
  BTSerial.println("\"");
}