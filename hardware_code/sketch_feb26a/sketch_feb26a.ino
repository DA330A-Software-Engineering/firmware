#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Set the communication address of I2C to 0x27, display 16 characters every line, two lines in total
LiquidCrystal_I2C mylcd(0x27, 16, 2);

// set ports of two servos to digital 9 and 10
Servo servo_10;
Servo servo_9;
volatile bool windowLock;
volatile bool doorLock;
volatile int previousMotion;
volatile int previousPhotocell;
volatile int previousSteam;
volatile int previousSoil;
volatile int previousGas;
void fanControl(int state, int rotation = 255);
void stateChange(int pin, int state, String state2 = "");
void screenControl(int state, String text = "");
void doorControl(int state, int lockedState = -1);
void windowControl(int state, int lockedState= -1);
String input;
bool screenOff;

void setup()
{
  Serial.begin(115200); // set baud rate to 9600

  mylcd.init();
  mylcd.backlight(); // initialize LCD
  // LCD shows "passcord:" at first row and column
  mylcd.setCursor(1 - 1, 1 - 1);
  mylcd.print("Welcome Home");

  servo_9.attach(9);   // make servo connect to digital 9
  servo_10.attach(10); // make servo connect to digital 10
  servo_9.write(0);    // set servo connected digital 9 to 0°
  servo_10.write(0);   // set servo connected digital 10 to 0°
  delay(300);

  pinMode(7, OUTPUT); // set digital 7 to output
  pinMode(6, OUTPUT); // set digital 6 to output

  pinMode(4, INPUT);   // set digital 4 to input
  pinMode(8, INPUT);   // set digital 8 to input
  pinMode(2, INPUT);   // set digital 2 to input
  pinMode(3, OUTPUT);  // set digital 3 to output
  pinMode(A0, INPUT);  // set A0 to input
  pinMode(A1, INPUT);  // set A1 to input
  pinMode(13, OUTPUT); // set digital 13 to input
  pinMode(A3, INPUT);  // set A3 to input
  pinMode(A2, INPUT);  // set A2 to input

  pinMode(12, OUTPUT); // set digital 12 to output
  pinMode(5, OUTPUT);  // set digital 5 to output
  pinMode(3, OUTPUT);  // set digital 3 to output
  
}

void loop(){

    while (Serial.available())
    {
      input = Serial.readString();
    }
    if (input != "")
    {
      if (getValue(input, ',', 2) == "")
      {
        stateChange(getValue(input, ',', 0).toInt(), getValue(input, ',', 1).toInt());
      }
      else
      {
        stateChange(getValue(input, ',', 0).toInt(), getValue(input, ',', 1).toInt(), getValue(input, ',', 2));
      }
      input = "";
    }
    
    while(Serial.available() == false){
      sensorChecker();
    }

}

void sensorChecker(){
  previousPhotocell = analogRead(A1);
  previousMotion = digitalRead(2);
  previousSteam = analogRead(A3);
  previousSoil = analogRead(A2);
  previousGas = analogRead(A0);
  delay(500);
  if (abs(previousPhotocell-analogRead(A1)) > 200){
  String returnMessage;
    returnMessage = "A1";
    returnMessage += ",";
    returnMessage += analogRead(A1);
    Serial.println(returnMessage);
  }
  
  if (digitalRead(2) != previousMotion){
  String returnMessage;
      returnMessage = 2;
      returnMessage += ",";
      returnMessage += digitalRead(2);
      Serial.println(returnMessage);
  }
  

  if (abs(previousSteam-analogRead(A3)) > 200){
  String returnMessage;
    returnMessage = "A3";
    returnMessage += ",";
    returnMessage += analogRead(A3);
    Serial.println(returnMessage);
  }

  

  if (abs(previousSoil-analogRead(A2)) > 100){
  String returnMessage;
    returnMessage = "A2";
    returnMessage += ",";
    returnMessage += analogRead(A2);
    Serial.println(returnMessage);
  }


  if (abs(previousGas-analogRead(A0)) > 100){
    String returnMessage;
    returnMessage = "A0";
    returnMessage += ",";
    returnMessage += analogRead(A0);
    Serial.println(returnMessage);
  }

}


/*
void photocellSensor(){
  
  previousPhotocell = analogRead(A1);
  delay(250);
  if (abs(previousPhotocell-analogRead(A1)) > 200){
    String returnMessage;
    returnMessage = "A1";
    returnMessage += ",";
    returnMessage += analogRead(A1);
    Serial.println(returnMessage);
  }
}

void motionSensor(){
  String returnMessage;
  previousMotion = digitalRead(2);
  delay(100);
  if (digitalRead(2) != previousMotion){
      returnMessage = 2;
      returnMessage += ",";
      returnMessage += digitalRead(2);
      Serial.println(returnMessage);
  }
}

void steamSensor(){
  String returnMessage;

  previousSteam = analogRead(A3);
  delay(250);
  if (abs(previousSteam-analogRead(A3)) > 200){
    returnMessage = "A3";
    returnMessage += ",";
    returnMessage += analogRead(A3);
    Serial.println(returnMessage);
  }
}

void soilSensor(){
  String returnMessage;

  previousSoil = analogRead(A2);
  delay(250);
  if (abs(previousSoil-analogRead(A2)) > 100){
    returnMessage = "A2";
    returnMessage += ",";
    returnMessage += analogRead(A2);
    Serial.println(returnMessage);
  }
}

void gasSensor(){
  String returnMessage;

  previousGas = analogRead(A0);
  delay(500);
  if (abs(previousGas-analogRead(A0)) > 100){
    returnMessage = "A0";
    returnMessage += ",";
    returnMessage += analogRead(A0);
    Serial.println(returnMessage);
  }
}

*/

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void stateChange(int pin, int state, String state2)
{
  switch (pin)
  {
  case 3:
    buzzerControl(state, state2.toInt());
    break;
  case 5:
    lightControl(pin, state);
    break;
  case 7:
    if (state2 != "")
    {
      fanControl(state, state2.toInt());
      break;
    }
    else
    {
      fanControl(state);
      break;
    }
  case 9:
    if(state2 != ""){
      doorControl(state, state2.toInt());
    }
    else{
      doorControl(state);
    }
    break;
  case 10:
    windowControl(state, state2.toInt());
    break;
  case 13:
    lightControl(pin, state);
    break;
  case 99:
    screenControl(state, state2);
    break;
  }
}

void buzzerControl(int duration, int frequency){
  tone(3, frequency);
  delay(duration);
  noTone(3);
}

void screenControl(int state, String text){
  if (state == 255)
  {
    mylcd.backlight();
  }
  else if (state == 0)
  {
    mylcd.noBacklight();
  }

  if (text != "")
  {
    mylcd.clear();
    mylcd.setCursor(1 - 1, 1 - 1);
    mylcd.print(text);
  }
}

void lightControl(int pin, int state)
{
  String returnMessage;
  if (state == 255)
  {
    digitalWrite(pin, HIGH);
    if(digitalRead(pin) == HIGH){
      returnMessage = pin;
      returnMessage += ",";
      returnMessage += state;
      Serial.println(returnMessage);
    }

  }
  else
  {
    digitalWrite(pin, LOW);
    if(digitalRead(pin) == LOW){
      returnMessage = pin;
      returnMessage += ",";
      returnMessage += state;
      Serial.println(returnMessage);
    }
  }
}

void windowControl(int state, int lockedState)
{
  if(lockedState != -1){
    windowLock = lockedState;
    Serial.print("lockedState = ");
    Serial.println(lockedState);
    Serial.print("doorLock = ");
    Serial.println(doorLock);

  }
  if (windowLock == 1)
  {
    screenControl(255, "Error. Locked!");
    delay(1000);
    screenControl(255, "Welcome home!");
  }
  else if (windowLock == 0)
  {
    if(state != -1 && state != -1){
      if(state == 255){
        servo_10.write(120);
      }
      else{
        servo_10.write(0);
      }
    }
  }
    Serial.print("State = ");
    Serial.println(state);
    Serial.print("lockedState = ");
    Serial.println(lockedState);
    Serial.print("windowLock = ");
    Serial.println(windowLock);
}

void doorControl(int state, int lockedState)
{
  if(lockedState != -1){
    doorLock = lockedState;
    Serial.print("lockedState = ");
    Serial.println(lockedState);
    Serial.print("doorLock = ");
    Serial.println(doorLock);

  }
  if (doorLock == 1)
  {
    screenControl(255, "Error. Locked!");
    delay(1000);
    screenControl(255, "Welcome home!");
  }
  else if (doorLock == 0)
  {
    if(state != -1 && state != -1){
      servo_9.write(state);
    }
  }
    Serial.print("State = ");
    Serial.println(state);
    Serial.print("lockedState = ");
    Serial.println(lockedState);
    Serial.print("doorLock = ");
    Serial.println(doorLock);
}

void fanControl(int state, int rotation)
{
  if ((state == 255) && (rotation == 255))
  {
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
  }
  else if ((state == 255) && (rotation == 0))
  {
    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);
  }
  else if (((state == 0) && (rotation == 0)) || (state == 0) && (rotation == 255))
  {
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
  }
}
