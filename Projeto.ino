#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h> 
#include "ACS712.h"

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

LiquidCrystal lcd(3,4,5,6,7,8);     //Instancia do LCD

ACS712 sensor(ACS712_05B, A0);

float tempo_inicial = 0;
float tempo_final = 0;
float tempo_total = 0;

double corrente = 0; 

double potencia = 0;
double energia = 0;
double energia_ant = 0;
double calculo = 0;

String conteudo= "";
String comparar = "";

bool liberou = false;

int tensao = 110;

float preco_Wh = 0.614320;

float consumo = 0;
float gasto = 0;

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);

  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  delay(2000);
  
  sensor.calibrate();
}

void loop() {
  
  if(liberou){
    corrente = -0.114;
    corrente += sensor.getCurrentAC(60);
    
    tempo_final = tempo_inicial;
    tempo_inicial = millis();
    tempo_total = tempo_inicial - tempo_final;
    
    potencia = tensao * corrente;
    
    energia = (energia + (potencia * (((tempo_total/60)/60)/10000)));


    Serial.print("Tempo: ");
    Serial.println(tempo_total);

    Serial.print("Ampere: ");
    Serial.println(corrente, 5);
    
    Serial.print("Potencia: ");
    Serial.println(potencia, 5);
  
    Serial.print("Energia: ");
    Serial.println((potencia * (((tempo_total/60)/60)/1000)), 5);
    Serial.println("----");

    //Limpa tela do LCD
    lcd.clear();
    //Imprime no LCD
    lcd.setCursor(0,0);
    lcd.print(corrente);
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
      if(conteudo.substring(1) == comparar){
        comparar = "";
        liberou = false;
        Serial.println("ACESSO ENCERRADO !!!");
        digitalWrite(2, HIGH);
        delay(500);

        consumo = energia;
        gasto = consumo * preco_Wh;
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Consumo: ");
        lcd.print(consumo);
        lcd.print("kWh");

        lcd.setCursor(0,1);
        lcd.print("Gasto: R$");
        lcd.print(gasto);

        delay(10000);
        
        energia = 0;
      }
      conteudo = "";
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
        comparar = conteudo.substring(1);
        conteudo = "";
        liberou = true;
        Serial.println("ACESSO LIBERADO !!!");

        //LIGA O RELÉ
        digitalWrite(2, LOW);
        delay(500);
      }
    }
  }
  delay(500);
}
