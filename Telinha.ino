#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);
int tempVal;
float volts;
float tempC;
float tempF;
int botao;
int celsius = 1;
void setup()
{
  Serial.begin(9600);
  pinMode(7,INPUT);
  lcd.begin(16, 2);
  lcd.clear();

}

void loop()
{
 lcd.print("Hello World!!!");
 Serial.println("Hello World!!!");
}
