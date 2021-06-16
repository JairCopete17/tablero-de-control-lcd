/*
   Tablero de control para los procesos de jugo de caña
   Realizado para una tarjeta STM32F103C8T6 (Blue Pill)
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
/*Creación de objeto LCD
   Conectar VCC -> 5V
            SCA -> PB7
            SCL -> PB6
*/
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define pailaUnoR PB12
#define pailaUnoV PB11
#define pailaDosR PB14
#define pailaDosV PB1
#define pailaTresR PA8
#define pailaTresV PA7
#define pailaCuatroR PA10
#define pailaCuatroV PA5
#define autR 60000

int estado = 0;
int posicion = 1, caldera = 1, litros = 0, grados = 0, valvula = 0;

byte separador[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, separador);
  lcd.clear();

  pinMode(pailaUnoR, OUTPUT);  pinMode(pailaDosR, OUTPUT);  pinMode(pailaTresR, OUTPUT); pinMode(pailaCuatroR, OUTPUT);
  pinMode(pailaUnoV, OUTPUT);  pinMode(pailaDosV, OUTPUT);  pinMode(pailaTresV, OUTPUT); pinMode(pailaCuatroV, OUTPUT);
}

void activarValvula (int act, bool red, bool green) {
  if (act == 1) {
    digitalWrite(pailaUnoR, red);
    digitalWrite(pailaUnoV, green);
  }
  else if (act == 2) {
    digitalWrite(pailaDosR, red);
    digitalWrite(pailaDosV, green);
  }
  else if (act == 3) {
    digitalWrite(pailaTresR, red);
    digitalWrite(pailaTresV, green);
  }
  else if (act == 4) {
    digitalWrite(pailaCuatroR, red);
    digitalWrite(pailaCuatroV, green);
  }
}

void inicio() {
  lcd.setCursor(1, 0);  lcd.print("Calde");
  lcd.setCursor(7, 0);  lcd.print("Lts");
  lcd.setCursor(11, 0); lcd.print("Temp");
  lcd.setCursor(16, 0); lcd.print("Val");
}

void bordes() {
  for (int i = 0; i <= 3; i++) {
    lcd.setCursor(0, i); lcd.write(0);
    lcd.setCursor(6, i); lcd.write(0);
    lcd.setCursor(10, i); lcd.write(0);
    lcd.setCursor(15, i); lcd.write(0);
    lcd.setCursor(19, i); lcd.write(0);
  }
}

void tablero (int pos, int cald, int lts, int temp, int val) {
  lcd.setCursor(3, pos);  lcd.print(cald);
  lcd.setCursor(7, pos);  lcd.print(lts);
  lcd.setCursor(11, pos); lcd.print(temp);
  lcd.setCursor(17, pos); lcd.print(val);
}

void loop() {
  inicio();
  bordes();
  activarValvula(1, true, false);
  activarValvula(2, true, false);
  activarValvula(3, true, false);
  activarValvula(4, true, false);
  while(1){
    tablero(posicion, caldera, litros, grados, valvula);
    //Primera etapa de producción
    if (caldera == 1 && estado == 0) {
      litros++;
      grados++;
      delay(180);
      //Limite del tamaño de caldera
      if (litros > 100) {litros = 100;}
      if (grados > 150) {grados = 150;}
      //Reposo y carga hacia la siguiente caldera
      if (litros == 100 && grados == 150) {
        tablero(posicion, caldera, 100, 150, valvula); 
        delay(autR);  //Reposo
        valvula = 1;
        activarValvula(1, false, true); //Encendido de led verde
        tablero(posicion, caldera, 100, 150, valvula);
        caldera = 2;
        estado = 1; //Carga hacia la siguiente caldera
      }
    }
    //Segunda etapa de producción
    if (caldera == 2 && estado == 1) {
      tablero(1, 1, 0, 0, 1);
      lcd.setCursor(11, 1); lcd.print("000"); //Corrimiento manual
      posicion = 2;
      valvula = 0;
      litros--;
      grados++;
      delay(250);
      //Limite del tamaño de caldera
      if (litros < 90) {litros = 90;}
      if (grados > 200) {grados = 200;}
      //Reposo y carga hacia la siguiente caldera
      if (litros == 90 && grados == 200) {
        tablero(posicion, caldera, 90, 200, valvula); 
        delay(autR);  //Reposo
        valvula = 1;
        activarValvula(2, false, true); //Encendido de led verde
        tablero(posicion, caldera, 90, 200, valvula);
        caldera = 4;
        estado = 2; //Carga hacia la siguiente caldera
      }
    }
    //Tercera etapa de producción
    if (caldera == 4 && estado == 2) {
      tablero(2, 2, 0, 0, 1);
      posicion = 3;
      valvula = 0;
      grados++;
      delay(300);
      //Limite del tamaño de caldera
      if (grados > 250) {grados = 250;}
      //Reposo y carga hacia la siguiente caldera
      if (litros == 90 && grados == 250) {
        tablero(posicion, caldera, 90, 250, valvula); 
        delay(autR);  //Reposo       
        valvula = 1;
        activarValvula(4, false, true); //Encendido de led verde
        tablero(posicion, caldera, 90, 250, valvula);
        caldera = 3;
        estado = 3; //Carga hacia la siguiente caldera
      }
    }
    //Ultima etapa de producción
    if (caldera == 3 && estado == 3) {
      tablero(3, 4, 0, 0, 1);
      lcd.setCursor(11, 3); lcd.print("000"); //Corrimiento manual
      posicion = 2;
      valvula = 0;
      grados++;
      delay(450);
      tablero(posicion, caldera, 90, 300, valvula);
      //Limite del tamaño de caldera
      if (grados > 300) {grados = 300;}
      //Reposo y carga hacia la siguiente caldera
      if (litros == 90 && grados == 300) {
        tablero(posicion, caldera, 90, 300, valvula); 
        delay(autR);  //Reposo       
        valvula = 1;
        activarValvula(3, false, true); //Encendido de led verde
        tablero(posicion, caldera, 90, 300, 1);
        estado = 4; //Carga hacia el reinicio del sistema
      }
    }
    //Reinicio del sistema
    if (estado == 4) {
      lcd.clear();
      posicion = 1;
      valvula = 0;
      //Carga hacia la primera caldera
      caldera = 1;  
      estado = 0;
      delay(500);
      return;   
    }
  }
}
