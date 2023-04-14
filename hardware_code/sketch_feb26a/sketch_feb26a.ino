#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NonBlockingRtttl.h>
#include <ezBuzzer.h>
// Set the communication address of I2C to 0x27, display 16 characters every line, two lines in total
LiquidCrystal_I2C mylcd(0x27, 16, 2);
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988

// set ports of two servos to digital 9 and 10
Servo servo_10;
Servo servo_9;
volatile bool windowLock;
volatile bool doorLock;
volatile bool playingMusic = false;
volatile int previousMotion;
volatile int previousPhotocell;
volatile int previousSteam;
volatile int previousSoil;
volatile int previousGas;
volatile bool tunePlaying;
void fanControl(int state, int rotation = 255);
void stateChange(int pin, String state, String state2 = "");
void screenControl(int state, String text = "");
void doorControl(int state, int lockedState = -1);
void windowControl(int state, int lockedState= -1);
String input;
String currDisplayText = "";
bool screenOff;

ezBuzzer buzzer(3);
String currSongID = "";


/*Music list*/
const char * pirate = "pirate:d=4,o=6,b=200:8d,8e,2f,8g,8a,g,f,e,f,g,a,g,p,8f,8g,a,p,8g,8f,e,f,e,d,p,8e,8c,d,8p,p,8d,8e,f,p,8e,8f,g,f,g,a,g,f,d";


// Music notes of the song, 0 is a rest/pulse
int pirateNotes[] = {
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,

    NOTE_A4, NOTE_A4,
    //Repeat of first part
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,
    //End of Repeat

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4,

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4};

// Durations (in ms) of each music note of the song
// Quarter Note is 4 ms when songSpeed = 1.0
int pirateDurations[] = {
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 2, 8,
    
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 2, 8,

    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 8, 4, 8,

    8, 8, 4, 8, 8,
    4, 8, 4, 8,
    8, 8, 4, 8, 8,
    8, 8, 2, 2,

    4, 8,
    //Rpeat of First Part
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 2, 8,

    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 2, 8,

    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 4, 8, 8,
    8, 8, 8, 4, 8,

    8, 8, 4, 8, 8,
    4, 8, 4, 8,
    8, 8, 4, 8, 8,
    8, 8, 2, 2,
    //End of Repeat

    4, 8, 2, 4, 8, 2,
    8, 8, 8, 8, 8, 8, 8, 8, 2,
    4, 8, 2, 4, 8, 2,
    8, 8, 8, 8, 8, 1,

    4, 8, 2, 4, 8, 2,
    8, 8, 8, 8, 8, 8, 8, 8, 2,
    4, 8, 2, 4, 8, 2,
    8, 8, 8, 8, 8, 1};

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
    buzzer.loop();

    while (Serial.available())
    {
      input = Serial.readString();
    }
    if (input != "")
    {
      if (getValue(input, ',', 2) == "")
      {
        stateChange(getValue(input, ',', 0).toInt(), getValue(input, ',', 1));
      }
      else
      {
        stateChange(getValue(input, ',', 0).toInt(), getValue(input, ',', 1), getValue(input, ',', 2));
      }
      input = "";
    }
    
    if(buzzer.getState() == BUZZER_IDLE && playingMusic) {
      playingMusic = false;      
      String returnMessage;
      returnMessage = 3;
      returnMessage += ",";
      returnMessage += "FIN";
      Serial.println(returnMessage);      
    }
    
    while(!Serial.available() && buzzer.getState() == BUZZER_IDLE){
      sensorChecker();
    }

}

void musicPlayer(String songID){
  if (songID == "pirate") {
    if (buzzer.getState() == BUZZER_IDLE) {
        buzzer.playMelody(pirateNotes, pirateDurations, sizeof(pirateNotes) / 2);
        playingMusic = true;
        String returnMessage;
        returnMessage = 3;
        returnMessage += ",";
        returnMessage += "pirate";
        Serial.println(returnMessage);
        return;
    }
  }  
  String returnMessage;
  returnMessage = 3;
  returnMessage += ",";
  returnMessage += "";
  Serial.println(returnMessage);
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
    returnMessage = "8";
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
    returnMessage = "6";
    returnMessage += ",";
    returnMessage += analogRead(A3);
    Serial.println(returnMessage);
  }

  

  if (abs(previousSoil-analogRead(A2)) > 100){
  String returnMessage;
    returnMessage = "4";
    returnMessage += ",";
    returnMessage += analogRead(A2);
    Serial.println(returnMessage);
  }


  if (abs(previousGas-analogRead(A0)) > 100){
    String returnMessage;
    returnMessage = "1";
    returnMessage += ",";
    returnMessage += analogRead(A0);
    Serial.println(returnMessage);
  }

}

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

void stateChange(int pin, String state, String state2)
{
  switch (pin)
  {
  case 3:
    musicPlayer(state);
    break;
  case 5:
    lightControl(pin, state.toInt());
    break;
  case 7:
    if (state2 != "")
    {
      fanControl(state.toInt(), state2.toInt());
    }
    else
    {
      fanControl(state.toInt());
    }
    break;
  case 9:
    if(state2 != ""){
      doorControl(state.toInt(), state2.toInt());
    }
    else{
      doorControl(state.toInt());
    }
    break;
  case 10:
    windowControl(state.toInt(), state2.toInt());
    break;
  case 13:
    lightControl(pin, state.toInt());
    break;
  case 99:
    screenControl(state.toInt(), state2);
    break;
  }
}

void buzzerControl(int duration, int frequency){
  tone(3, frequency);
  delay(duration);
  noTone(3);
}

void screenControl(int state, String text){
  String returnMessage;
  if (state == 255)
  {
    mylcd.backlight();
    screenOff = false;
    returnMessage = "99";
    returnMessage += ",";
    returnMessage += "255";
    returnMessage += ",";
    returnMessage += currDisplayText;
    Serial.println(returnMessage);
  }
  else if (state == 0)
  {
        
    mylcd.noBacklight();
    screenOff = true;
    returnMessage = "99";
    returnMessage += ",";
    returnMessage += "0";
    returnMessage += ",";
    returnMessage += currDisplayText;
    Serial.println(returnMessage);
  }

  if (text != "")
  {
    currDisplayText = text;
    mylcd.clear();
    mylcd.setCursor(1 - 1, 1 - 1);
    mylcd.print(text);
    returnMessage = "99";
    returnMessage += ",";
    returnMessage += screenOff;
    returnMessage += ",";
    returnMessage += currDisplayText;
    Serial.println(returnMessage);
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
  String returnMessage;
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
    returnMessage = "10";
    returnMessage += ",";
    returnMessage += state;
    returnMessage += ",";
    returnMessage += "255";
    Serial.println(returnMessage);
  }
  else if (windowLock == 0)
  {
    if(state != -1){
      if(state == 255){
        servo_10.write(120);
      }
      else{
        servo_10.write(0);
      }
    }
    returnMessage = "10";
    returnMessage += ",";
    returnMessage += state;
    returnMessage += ",";
    returnMessage += "0";
    Serial.println(returnMessage);
  }
}

void doorControl(int state, int lockedState)
{
  String returnMessage;
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
    returnMessage = "9";
    returnMessage += ",";
    returnMessage += state;
    returnMessage += ",";
    returnMessage += "255";
    Serial.println(returnMessage);
  }
  else if (doorLock == 0)
  {
    if(state != -1){
      servo_9.write(state);
    }
    returnMessage = "9";
    returnMessage += ",";
    returnMessage += state;
    returnMessage += ",";
    returnMessage += "0";
    Serial.println(returnMessage);
  }
}

void fanControl(int state, int rotation)
{
  String returnMessage;
  if ((state == 255) && (rotation == 255))
  {
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
    returnMessage = "7";
    returnMessage += ",";
    returnMessage += "255";
    returnMessage += ",";
    returnMessage += "255";
    Serial.println(returnMessage);
  }
  else if ((state == 255) && (rotation == 0))
  {
    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);
    returnMessage = "7";
    returnMessage += ",";
    returnMessage += "255";
    returnMessage += ",";
    returnMessage += "0";
    Serial.println(returnMessage);
  }
  else if (((state == 0) && (rotation == 0)) || (state == 0) && (rotation == 255))
  {
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    returnMessage = "7";
    returnMessage += ",";
    returnMessage += "0";
    returnMessage += ",";
    returnMessage += rotation;
    Serial.println(returnMessage);
  }  
}
