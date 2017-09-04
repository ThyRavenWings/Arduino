/*
LCD
 *magistrala
 pin2 D7
 pin3 D6
 pin4 D5
 pin5 D4

 pin12 RS
 pin11 E
 GND RW
 GND VSS
 5V VCC
 Dzielnik napięcia V0

Servo
 5V czerwony
 GND czarny
 pin7 żółty

DS18B20
  5V czerwony
  GND czarny
  pin6 tentrzeci

przycisk1
 5V i pin8
przycisk2
 5V i pin9
przycisk3
 5V i pin10

 
 */

#include <LiquidCrystal.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define MAGISTRALA 6
#define SERWO_PIN 7 //albo przekaznik

#define PLUS 8
#define MINUS 9
#define MODE 10

float* chciana_temp = NULL;
float* histereza = NULL;
bool* tryb =NULL;                      //serwo = true      przekaznik = false
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo serwo;
int i;
OneWire onewire(MAGISTRALA);
DallasTemperature termometr(&onewire);


bool przycisk(int pin);
void opcje(int przycisk1,int przyciskplus,int przyciskminus,LiquidCrystal lcd,float *chciana_temp , float *histereza , bool *tryb);
float tryb_serwa(Servo serwo, DallasTemperature termometr, LiquidCrystal lcd);
float tryb_przekaznika(int przekaznik, DallasTemperature termometr, LiquidCrystal lcd);



void setup() {
  
  *chciana_temp = 78.2;
  *histereza = 0.5;
  *tryb=false;

  pinMode(PLUS,INPUT_PULLUP);
  pinMode(MINUS,INPUT_PULLUP);
  pinMode(MODE,INPUT_PULLUP);
  lcd.begin(16, 2);
  serwo.attach(SERWO_PIN);
  serwo.write(0);
  delay(500);
  termometr.begin();
  Serial.begin(9600);
  }

void loop()
{ 
  if(*tryb)
  {
    tryb_serwa(serwo, termometr, lcd,chciana_temp,histereza);
    lcd.setCursor(0,0);
    lcd.print("Tryb serwa      ");
  }
  else
  {
    tryb_przekaznika(7,termometr,lcd,chciana_temp,histereza);
     lcd.setCursor(0,0);
     lcd.print("Tryb przekaźnika");
  }
  for(i=0;i<25;i++)
  {
    if(przycisk(MODE))
    {
      opcje(MODE,PLUS,MINUS,lcd,chciana_temp,histereza,tryb);
      break;
    }
    delay(5);
  }
  
}

bool przycisk(int pin)
{
  if(digitalRead(pin) == LOW)
  {
    delay(30);
    while(digitalRead(pin) == LOW);
    delay(30);
    return true;
  }
  else
    return false;
}

void opcje(int przycisk1,int przyciskplus,int przyciskminus,LiquidCrystal lcd,float *chciana_temp , float *histereza , bool *tryb)
{
    lcd.begin(16,2);
    int f=2;
    lcd.setCursor(0,0);
    lcd.print("+ Tryb     ");
    lcd.setCursor(0,1);
    lcd.print("- Parametry    ");
      while(f==2)
      {
       if(przycisk(przyciskplus))
          f=1;
        else if(przycisk(przyciskminus))
          f=0;
        else
          f=2;
      }
    if(f==0)
    {
      f=2;
      lcd.setCursor(0,0);
      lcd.print("+ Serwo     ");
      lcd.setCursor(0,1);
      lcd.print("- Przekaźnik       ");
      while(f==2)
      {
       if(przycisk(przyciskplus))
          f=0;
        else if(przycisk(przyciskminus))
         f=1;
        else
          f=2;
      }
      if(f==0)
      {
        *tryb=true;
      }
      else
      {
        *tryb=false;
      }
    }
    else
    {
      bool z=true;
      while(!przycisk(przycisk1))
      {
        if(z)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Temperatura   ");
          lcd.setCursor(0,1);
          lcd.print(*chciana_temp);
        }
        if(przycisk(przyciskplus))
        {
          (*chciana_temp) += 0.1;
          z=true;
        }
        else if(przycisk(przyciskminus))
        {
          (*chciana_temp) -= 0.1;
          z=true;
        }
        else
          z=false;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Histereza   ");
      while(!przycisk(przycisk1))
      {
        lcd.setCursor(0,1);
        lcd.print(*histereza);
        if(przycisk(przyciskplus))
          (*histereza) += 0.1;
        else if(przycisk(przyciskminus))
          (*histereza) -= 0.1;
      }
      
    }
}
float tryb_serwa(Servo serwo, DallasTemperature termometr, LiquidCrystal lcd,float *chciana_temp,float* histereza)
{
  float temp;
  lcd.setCursor(0, 1);
  lcd.print("T =         ");
  termometr.requestTemperatures();
  temp = termometr.getTempCByIndex(0);
  lcd.print(temp);
  if(temp-*chciana_temp>*histereza)
  {
    serwo.write(90);
  }
  else if(*chciana_temp-temp>*histereza)
  {
    serwo.write(0);
  }
  return temp;
}
float tryb_przekaznika(int przekaznik, DallasTemperature termometr, LiquidCrystal lcd,float *chciana_temp,float* histereza)
{
  float temp;
  lcd.setCursor(0, 1);
  lcd.print("T = ");
  termometr.requestTemperatures();
  temp = termometr.getTempCByIndex(0);
  lcd.print(temp);
  if(temp-*chciana_temp>*histereza)
  {
    digitalWrite(przekaznik,LOW);
  }
  else if(*chciana_temp-temp>*histereza)
  {
    digitalWrite(przekaznik,HIGH);
  }
  return temp;
}
