#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);

long unsigned int tempo_inicial, tempo_final, tempo_total;
float vetCorrente[300];
float tensao = 127;
double corrente = 0;
double maior_Valor = 0;
double valor_Corrente = 0; 
double potencia = 0;
double energia = 0;

void setup() {
	Serial.begin(9600);
	pinMode(A0, INPUT);
	lcd.begin(16, 2);

}

void loop() {
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

	tempo_final = tempo_inicial;
	tempo_inicial = milis();
	tempo_total = tempo_final - tempo_inicial;

	potencia = 110 * valor_Corrente;
	energia += potencia * (tempo_total/60/60/1000);

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

	//Imprime no Terminal
	Serial.print("Corrente = ");
	Serial.print(valor_Corrente);
	Serial.println(" A");

	Serial.print("Potencia = ");
	Serial.print(potencia);
	Serial.println(" W");

	Serial.print("Energia = ");
	Serial.print(energia);
	Serial.println(" kWh");

	delay(500);
}
