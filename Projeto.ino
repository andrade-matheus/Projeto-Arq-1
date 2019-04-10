#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h> 
#include "ACS712.h"

#define SS_PIN 10
#define RST_PIN 9
#define rele 2

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Instancia do RFID
LiquidCrystal lcd(3,4,5,6,7,8);     //Instancia do LCD
ACS712 sensor(ACS712_05B, A0);      //Instancia Sensor de Corrente

float tempo_inicial = 0;
float tempo_final = 0;
float tempo_total = 0;
float preco_Wh = 0.614320;
float consumo = 0;
float gasto = 0;
double corrente = 0; 
double potencia = 0;
double energia = 0;
double energia_ant = 0;
double calculo = 0;
String conteudo= "";
String comparar = "";
bool liberou = false;
int tensao = 110;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH); //Desliga o relé
  sensor.calibrate();
  delay(500);
}

void loop() {
  if(liberou){ //Ao liberar o acesso começa a leitura do consumo de energia
    corrente = -0.114;
    corrente += sensor.getCurrentAC(60);
    
    //Calculo do tempo de leitura da corrente
    tempo_final = tempo_inicial;
    tempo_inicial = millis();
    tempo_total = tempo_inicial - tempo_final;
    
    //Calculo da potencia e energia
    potencia = tensao * corrente;
    energia = (energia + (potencia * (((tempo_total/60)/60)/10000)));

    lcd.clear(); //Limpa tela do LCD
    
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
   
    if (mfrc522.PICC_IsNewCardPresent()) {  // Procura pelos Cartões
      if (mfrc522.PICC_ReadCardSerial()) {  // Seleciona um dos cartões
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
          conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        conteudo.toUpperCase();    
      }
      if(conteudo.substring(1) == comparar){
        liberou = false; //Encerra o acesso
        digitalWrite(rele, HIGH); //Desliga o relé
       
        //Calculo do consumo de energia em R$
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
        delay(10000); //Delay para que o preço fique 10 segundo no LCD
        
        //Reinicia algumas variaveis
        energia = 0;
        comparar = "";
      }
      conteudo = "";
    }
  } else { //Caso acesso não esteja liberado, somente procura por novos cartões
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Aproxime o");
    lcd.setCursor(0,1);
    lcd.print("Cartao de Acesso");
    
    if (mfrc522.PICC_IsNewCardPresent()) { // Procura pelos Cartões
      if (mfrc522.PICC_ReadCardSerial()) { // Seleciona um dos cartões
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
          conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        conteudo.toUpperCase();
      }
      if((conteudo.substring(1) == "FB C0 4C 0C") || (conteudo.substring(1) == "16 DB 28 F8")){
        liberou = true; //Libera o Acesso
        digitalWrite(rele, LOW); //Liga o relé
        delay(500);
        
        //Reinicia algumas variaveis
        comparar = conteudo.substring(1);
        conteudo = "";
      }
    }
  }
  delay(500);
}
