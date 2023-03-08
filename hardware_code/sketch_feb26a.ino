#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Set the communication address of I2C to 0x27, display 16 characters every line, two lines in total
LiquidCrystal_I2C mylcd(0x27, 16, 2);

// set ports of two servos to digital 9 and 10
Servo servo_10;
Servo servo_9;
volatile bool locked = true;
void fanControl(int state, int rotation = 255);
void stateChange(int pin, int state, String state2 = "");
void screenControl(int state, String text = "");
String input;
bool screenOff;

void setup()
{
  Serial.begin(9600); // set baud rate to 9600

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

void loop()
{
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
}

String getValue(String data, char separator, int index)
{
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
    // statements
    break;
  case 5:
    lightControl(pin, state);
    break;
  case 7:
    if (state2 != "")
    {
      fanControl(state, state2.toInt());
    }
    else
    {
      fanControl(state);
    }
  case 9:
    // statements
    break;
  case 10:
    windowControl(locked, state);
    break;
  case 13:
    lightControl(pin, state);
    break;
  case 99:
    screenControl(state, state2);
    break;
  }
}

void screenControl(int state, String text)
{
  Serial.print("STATE = ");
  Serial.println(state);
  Serial.print("TEXT = ");
  Serial.println(text);
  if (state == 255)
  {
    mylcd.backlight();
  }
  else if (state == 0)
  {
    mylcd.noBacklight();
    Serial.print("reached");
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
  if (state == 255)
  {
    digitalWrite(pin, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  }
}

void windowControl(bool locked, int state)
{
  if (locked)
  {
    Serial.print("print 'window locked' to screen");
  }
  else
  {
    servo_9.write(state);
  }
}

void fanControl(int state, int rotation)
{
  Serial.print(state);
  Serial.print(" ");
  Serial.println(rotation);
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
