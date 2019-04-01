#include "ACS712.h"
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal lcd(3,4,5,6,7,8);

ACS712 sensor(ACS712_30A, A0);

float tempo_inicial = 0;
float tempo_final = 0;
float tempo_total = 0;
float vetCorrente[300];
float tensao = 127;
double corrente = 0;
double maior_Valor = 0;
double valor_Corrente = 0; 
double potencia = 0;
double energia = 0;
double energia_ant = 0;
double calculo = 0;

String conteudo= "";
String comparar = "";

bool liberou = false;

void setup() {
  Serial.begin(9600);

  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  sensor.calibrate();
  Serial.println("Done!");
  
  lcd.begin(16, 2);
 
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
}

void loop() {
  if(liberou){
    /*
    for(int i = 0; i < 300; i++) {
      corrente = analogRead(A0);
      if(corrente > maior_Valor) {
        maior_Valor = corrente;
      }
    }
  
    maior_Valor = maior_Valor * 0.004882812;
    valor_Corrente = maior_Valor - 2.5;
    valor_Corrente = valor_Corrente * 1000;
    valor_Corrente = valor_Corrente / 66;         //sensibilidade : 66mV/A para ACS712 30A / 185mV/A para ACS712 5A
    valor_Corrente = valor_Corrente / 1.41421356;
    */
    
    valor_Corrente = sensor.getCurrentAC();
  
    tempo_final = tempo_inicial;
    tempo_inicial = millis();
    tempo_total = tempo_inicial - tempo_final;
    
    potencia = 100 * valor_Corrente;
    
    energia += (potencia * (tempo_total/60/60/1000));

    //Limpa tela do LCD
    lcd.clear();
    //Imprime no LCD
    lcd.setCursor(0,0);
    lcd.print(valor_Corrente);
    lcd.print(" Amp");
    
    lcd.setCursor(9,0);
    lcd.print(potencia);
    lcd.print(" W");
    
    lcd.setCursor(0,1);
    lcd.print(energia);
    lcd.print(" kWh");
    
    // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent()) {
      // Select one of the cards
      if (mfrc522.PICC_ReadCardSerial()) {
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
          conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        conteudo.toUpperCase();    
      }
      if((conteudo.substring(1) == "FB C0 4C 0C") || (conteudo.substring(1) == "16 DB 28 F8")){
        energia = 0;
        conteudo = "";
        liberou = false;
        Serial.println("ACESSO ENCERRADO !!!");
        digitalWrite(2, LOW);
        delay(500);
      }
    }
  } else {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Aproxime o");
    lcd.setCursor(0,1);
    lcd.print("Cartao de Acesso");
    
    // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent()) {
      // Select one of the cards
      if (mfrc522.PICC_ReadCardSerial()) {
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
          conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        conteudo.toUpperCase();
      }
      if((conteudo.substring(1) == "FB C0 4C 0C") || (conteudo.substring(1) == "16 DB 28 F8")){
        conteudo = "";
        liberou = true;
        Serial.println("ACESSO LIBERADO !!!");
        digitalWrite(2, HIGH);
        delay(500);
      }
    }
  }
  delay(500);
}
