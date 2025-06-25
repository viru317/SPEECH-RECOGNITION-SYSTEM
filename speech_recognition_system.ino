#include <Keypad.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
LiquidCrystal lcd2(44, 45, 46, 47, 48, 49);

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'7','8','9','/'},
  {'4','5','6','*'},
  {'1','2','3','-'},
  {'.','0','=','+'}
};

byte rowPins[ROWS] = {50, 51, 52, 53};
byte colPins[COLS] = {A0, A1, A2, A3};

String password="*124";
String check="";
int currentposition=0;
bool flag=0;
const int buzzer1 = 14;
int f = 0;
int ldr = A6;
int value = 0;
int temp=0;
int lm35=A5;
int wrongcount=0;
int pirpin=A8;
int pirStat=0;

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String voice;

int fan = 5, fan2 = 4;
int light = 6, light2 = 7;

#define MQPin A4
#define buzzer2 1

int resval = 0;
int respin = A7;

void setup()
{
  Serial2.begin(9600);
  pinMode(light, OUTPUT);
  pinMode(light2, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(MQPin, INPUT_PULLUP);
  pinMode(buzzer2, OUTPUT);
  pinMode(pirPin, INPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Loading/Initializing");
  lcd.clear();
  lcd.setCursor(0, 0);
  pinMode(buzzer1, OUTPUT);
  lcd2.begin(16, 2);
}

void loop()
{
  char customKey = customKeypad.getKey();
  //get pir sensor value
  pirStat=digitalRead(pirPin);
  if (customKey)
  {
    //append string if press other keys
    if(customKey!='=' && customKey!='.')
    {
      check=check+customKey;
      lcd.clear();
      lcd.begin(16, 2);
      lcd.print(check);
    }
    //clean password screen
    else if(customKey=='.')
    {
      check="";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(check);
    }
    else
    {
      //welcome if matched
      if(check==password)
      {
        lcd.clear();
        lcd.begin(16, 2);
        lcd.print("Welcome!");
        wrongcount=0;
        //The person enters the house
        f = 1;
      }
      else
      {
        wrongcount++;
        if(wrongcount<=2)
        {
          lcd.clear();
          lcd.begin(16, 2);
          lcd.print("Wrong Password!");
        }
      }
      if(wrongcount>2)
      {
        lcd.clear();
        lcd.begin(16, 2);
        lcd.print("Intruder Alert!!! Message Sent!");
      }
      check="";
    }
  }
  //after enter
  if(f == 1)
  {
    //check voice command
    while(Serial2.available())
    {
      delay(3);
      char c = Serial2.read();
      voice += c;
    }
    //voice="turn on fan";
    if(voice.length() > 0)
    {
      Serial2.println(voice);
      if(voice == "turn on light")
      {
        value=analogRead(ldr);
        if(value<=500)
        {
          digitalWrite(light, HIGH);
          digitalWrite(light2, HIGH);
        }
        else
        {
          digitalWrite(light, HIGH);
          digitalWrite(light2, LOW);
        }
      }
      else if(voice == "turn off light")
      {
        digitalWrite(light, LOW);
        digitalWrite(light2, LOW);
      }
      else if(voice == "turn on fan")
      {
        temp=analogRead(lm35);
        float milivolts=(temp/1024.0)*5*1000;
        float cel=milivolts/10;
        float farh=(cel*9)/5+32;
        if(cel<27)
        {
          digitalWrite(fan, HIGH);
          digitalWrite(fan2, LOW);
        }
        else
        {
          digitalWrite(fan, HIGH);
          digitalWrite(fan2, HIGH);
        }
      }
      else if(voice == "turn off fan")
      {
        digitalWrite(fan, LOW);
        digitalWrite(fan2, LOW);
      }
      else if(voice == "turn on all")
      {
        value=analogRead(ldr);
        if(value<=500)
        {
          digitalWrite(light, HIGH);
          digitalWrite(light2, HIGH);
        }
        else
        {
          digitalWrite(light, HIGH);
          digitalWrite(light2, LOW);
        }

        //get temperature sensor value
        temp=analogRead(lm35);
        float milivolts=(temp/1024.0)*5*1000;
        float cel=milivolts/10;
        float farh=(cel*9)/5+32;
        if(cel<27)
        {
          digitalWrite(fan, HIGH);
          digitalWrite(fan2, LOW);
        }
        else
        {
          digitalWrite(fan, HIGH);
          digitalWrite(fan2, HIGH);
        }
      }
      else if(voice == "turn off all")
      {
        digitalWrite(fan, LOW);
        digitalWrite(light, LOW);
      }
      voice = "";
     }

     //get gas sensor value
     int gas_value = digitalRead(MQPin);
     if(gas_value==HIGH)
     {
       digitalWrite(buzzer2, HIGH);
       //fan on to emit gas from room
       digitalWrite(fan, HIGH);
       digitalWrite(fan2, HIGH);
       delay(200);
     }
     else
     {
       digitalWrite(buzzer2, LOW); 
     }

     lcd2.print("  WATER LEVEL : ");
     lcd2.setCursor(0, 1);

     //get water sensor value
     resval = analogRead(respin);
     if (resval<=100)
     {
        lcd2.println("     Empty    ");
     } 
     else if (resval>100 && resval<=300)
     {
        lcd2.println("       Low      ");
     }
     else if (resval>300 && resval<=330)
     {
        lcd2.println("     Medium     ");
     } 
     else if (resval>330)
     {
        lcd2.println("      High      ");
        tone(buzzer1, 1000);
        delay(1000); 
        noTone(buzzer1);
        delay(1000);
     }
     
  }
  // not signed but motion detected
  else if(f==0)
  {
    if(pirStat==HIGH)
    {
      lcd.clear();
      lcd.begin(16,2);
      lcd.print("Unauth Motion Detected!");
      tone(buzzer1, 1000);
      delay(1000); 
      noTone(buzzer1);
      delay(1000);  
    }
  }
}
