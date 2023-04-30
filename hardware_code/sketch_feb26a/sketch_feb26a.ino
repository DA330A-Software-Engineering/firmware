#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NonBlockingRtttl.h>
#include <ezBuzzer.h>
// Set the communication address of I2C to 0x27, display 16 characters every line, two lines in total
LiquidCrystal_I2C mylcd(0x27, 16, 2);
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_DS4 311
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_GS4 415
#define NOTE_AS4 466
#define NOTE_CS5 554
#define NOTE_DS5 622
#define NOTE_FS5 740
#define NOTE_GS5 831
#define NOTE_AS5 932
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
#define REST 0

// set ports of two servos to digital 9 and 10
Servo servo_10;
Servo servo_9;
volatile bool windowLock;
volatile bool windowOpen;
volatile bool doorLock;
volatile bool doorOpen;
volatile bool playingMusic = false;
volatile int previousMotion;
volatile int previousPhotocell;
volatile int previousSteam;
volatile int previousSoil;
volatile int previousGas;
volatile int currMotion;
volatile int currPhotocell;
volatile int currSteam;
volatile int currSoil;
volatile int currGas;

void fanControl(int state, int rotation = -1);
void stateChange(int pin, String state, String state2 = "");
void screenControl(int state, char* text = "");
void doorControl(int state, int lockedState = -1);
void windowControl(int state, int lockedState= -1);
String input;
char* currDisplayText = "";
bool screenOff = false;
bool fanReverse = false;
bool fanOn = false;

ezBuzzer buzzer(3);
String currSongID = "";

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
  8, 8, 8, 8, 8, 1
};

int alarmNotes[] = {
  NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6, REST, NOTE_CS6,
};

int alarmDurations[] = {
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
};

void setup()
{
  Serial.begin(115200); // set baud rate to 115200

  mylcd.init();
  mylcd.backlight(); // initialize LCD
  // LCD shows "passcord:" at first row and column
  mylcd.setCursor(0, 0);
  mylcd.print("Welcome Home!");

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
      sendToSerial("3,FIN");
    }
    
    while(!Serial.available() && buzzer.getState() == BUZZER_IDLE){
      sensorChecker();
    }

}

void sendToSerial(char* msg) {
  Serial.println(msg);
}

void musicPlayer(String &songID){
  if (buzzer.getState() != BUZZER_IDLE) {
    return;
  }

  if (songID == "pirate") {
      buzzer.playMelody(pirateNotes, pirateDurations, sizeof(pirateNotes) / 2);
      playingMusic = true;
      sendToSerial("3,none");
      sendToSerial("3,pirate");
      return;
  } else if (songID == "alarm") {
      buzzer.playMelody(alarmNotes, alarmDurations, sizeof(alarmNotes) / 2);
      playingMusic = true;
      sendToSerial("3,none");
      sendToSerial("3,alarm");
      return;
  }
  sendToSerial("3,none");
}

void sensorChecker(){
  previousPhotocell = analogRead(A1);
  previousMotion = digitalRead(2);
  previousSteam = analogRead(A3);
  previousSoil = analogRead(A2);
  previousGas = analogRead(A0);

  char buff[30] = "";
  char valueBuff[10] = "";

  delay(500);
  if (abs(previousPhotocell-analogRead(A1)) > 200){
    strcpy(buff, "8,");
    itoa(analogRead(A1), valueBuff, 10);
    strcat(buff, valueBuff);
    sendToSerial(buff);
  }
  
  if (digitalRead(2) != previousMotion){
    strcpy(buff, "2,");
    strcat(buff, digitalRead(2) ? "1023" : "0");
    sendToSerial(buff);
  }
  

  if (abs(previousSteam-analogRead(A3)) > 200){
    strcpy(buff, "6,");
    itoa(analogRead(A3), valueBuff, 10);
    strcat(buff, valueBuff);
    sendToSerial(buff);
  }

  

  if (abs(previousSoil-analogRead(A2)) > 100){
    strcpy(buff, "4,");
    itoa(analogRead(A2), valueBuff, 10);
    strcat(buff, valueBuff);
    sendToSerial(buff);
  }


  if (abs(previousGas-analogRead(A0)) > 100){
    strcpy(buff, "1,");
    itoa(analogRead(A0), valueBuff, 10);
    strcat(buff, valueBuff);
    sendToSerial(buff);
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
    if (state2 == "" || state2.toInt() == -1) {
      screenControl(state.toInt());
    } else {
      char screenTextBuff[16] = "";
      strncpy(screenTextBuff, state2.c_str(), 16);
      screenTextBuff[16] = '\0';
      screenControl(state.toInt(), screenTextBuff);
    }
    break;
  }
}

void writeScreen(char* text) {
  mylcd.setCursor(0, 1);
  mylcd.print(text);
}

void screenControl(int state, char* text){
  if (state != -1) {
    if (state == 255) {
      screenOff = false;
    } else {
      screenOff = true;
    }
  }  

  currDisplayText = text;
  
  if (screenOff)
  {
    mylcd.noBacklight();
  }
  else
  {
    mylcd.backlight();
  }
  
  mylcd.clear();
  mylcd.setCursor(1 - 1, 1 - 1);
  mylcd.print(currDisplayText);

  char buff[30] = "";
  strcpy(buff, "99,");
  strcat(buff, screenOff ? "0" : "255");
  strcat(buff, ",");
  strcat(buff, currDisplayText);

  sendToSerial(buff);
}

void lightControl(int pin, int state)
{
  char buff[30] = "";
  char valueBuff[10] = "";

  if (state == 255)
  {
    digitalWrite(pin, HIGH);

  }
  else
  {
    digitalWrite(pin, LOW);
  }

  if (digitalRead(pin) == HIGH) {
    itoa(pin, buff, 10);
    strcat(buff, ",255");
    sendToSerial(buff);
    return;
  }

  itoa(pin, buff, 10);
  strcat(buff, ",0");
  sendToSerial(buff); 
}

void windowControl(int state, int lockedState)
{
  char buff[30] = "";

  if(lockedState != -1){
    windowLock = lockedState;
  }

  if (windowLock == 1)
  {
    writeScreen("Error. Locked!");
    delay(1000);
    writeScreen("Welcome home!");

    strcpy(buff, "9,");
    strcat(buff, windowOpen ? "255" : "0");
    strcat(buff, ",255");
    sendToSerial(buff);
  }
  else if (windowLock == 0)
  {
    if(state != -1){
      windowOpen = state == 255;
    }

    servo_9.write(windowOpen ? 255 : 0);
    strcpy(buff, "9,");
    strcat(buff, windowOpen ? "255" : "0");
    strcat(buff, ",0");
    sendToSerial(buff);
  }
}

void doorControl(int state, int lockedState)
{
  char buff[30] = "";

  if(lockedState != -1){
    doorLock = lockedState;
  }

  if (doorLock == 1)
  {
    writeScreen("Error. Locked!");
    delay(1000);
    writeScreen("Welcome home!");

    strcpy(buff, "9,");
    strcat(buff, doorOpen ? "255" : "0");
    strcat(buff, ",255");
    sendToSerial(buff);
  }
  else if (doorLock == 0)
  {
    if(state != -1){
      doorOpen = state == 255;
    }

    servo_9.write(doorOpen ? 255 : 0);
    strcpy(buff, "9,");
    strcat(buff, doorOpen ? "255" : "0");
    strcat(buff, ",0");
    sendToSerial(buff);
  }
}

void fanControl(int state, int rotation)
{
  if(rotation != -1) {
    if (rotation == 255) {
      fanReverse = true;
    } else if (rotation == 0) {
      fanReverse = false;
    }
  }

  if (state != -1) {
    if (state == 255) {
      fanOn = true;
    } else {
      fanOn = false;
    }
  }
  
  if (fanOn) {
    if (fanReverse) {
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
      sendToSerial("7,255,255");
    } else {
      digitalWrite(7, LOW);
      digitalWrite(6, HIGH);
      sendToSerial("7,255,0");
    }
  } else {
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    char buff[30] = "";
    strcpy(buff, "7,0,");
    strcat(buff, fanReverse ? "255" : "0");

    sendToSerial(buff);
  }
}
