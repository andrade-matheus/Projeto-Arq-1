#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 6
#define RST_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal lcd(13,12,11,10,9,8);

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

void RFID(){
  Serial.println("TESTE RFID");
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  
  
  while(conteudo.substring(1) != "FB C0 4C 0C" || conteudo.substring(1) != "16 DB 28 F8"){
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Mensagem : ");
    conteudo.toUpperCase();
    Serial.println(conteudo.substring(1)); 
  }
  digitalWrite(7, HIGH);  
}

void setup() {
	Serial.begin(9600);
	pinMode(A3, INPUT);
	lcd.begin(16, 2);
 
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  pinMode(7, OUTPUT);

  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  
  RFID();
}

void loop() {
  Serial.println("TESTE LOOP");
	for(int i = 0; i < 300; i++) {
		corrente = analogRead(A2);
		if(corrente > maior_Valor) {
			maior_Valor = corrente;
		}
	}

	maior_Valor = maior_Valor * 0.004882812;
	valor_Corrente = maior_Valor - 2.5;
	valor_Corrente = valor_Corrente * 1000;
	valor_Corrente = valor_Corrente / 66;         //sensibilidade : 66mV/A para ACS712 30A / 185mV/A para ACS712 5A
	valor_Corrente = valor_Corrente / 1.41421356;

	tempo_final = tempo_inicial;
	tempo_inicial = millis();
	tempo_total = tempo_inicial - tempo_final;
  
	potencia = 110 * valor_Corrente;
  
	energia += (potencia * (tempo_total/60/60/1000));
  
	//Imprime no LCD
	lcd.setCursor(0,0);
	lcd.print(valor_Corrente);
	lcd.print(" A");
	
	lcd.setCursor(8,0);
	lcd.print(potencia);
	lcd.print(" W");
	
	lcd.setCursor(0,1);
	lcd.print(energia);
	lcd.print(" kWh");

	delay(500);

  Serial.println("TESTE LOOP 2");
  
  byte letra;
  String conteudo_loop = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.println("TESTE LOOP 3");
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo_loop.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo_loop.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println("TESTE LOOP 4");
  
  if(conteudo_loop.equals(conteudo)){
    digitalWrite(7, LOW);
    delay(1000);
    RFID();     
  }

  Serial.println("TESTE LOOP 5");
  
}
