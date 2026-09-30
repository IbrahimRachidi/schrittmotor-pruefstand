// Prüfstand für Schrittmotoren – Arduino Mega 2560 + DRV8825
// Funktionen: Spulenprüfung, Drehrichtungstest, Drehzahlverstellung, Frequenzanzeige

#include <LiquidCrystal.h>
const int rs = 22, en = 13, d4 = 24, d5 = 25, d6 = 26, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);   // Pins des LCD

// Erster Spannungsteiler (Spule 1)
const int sensorPin1 = A1;
int   sensorValue1 = 0;     // Messwert des Analogeingangs
float Ve1 = 5;              // Eingangsspannung
float Vs1 = 0;              // Ausgangsspannung
float Rref1 = 1000;         // Referenzwiderstand
float R1 = 0;               // Gemessener Spulenwiderstand

// Zweiter Spannungsteiler (Spule 2)
const int sensorPin2 = A2;
int   sensorValue2 = 0;
float Ve2 = 5;
float Vs2 = 0;
float Rref2 = 1000;
float R2 = 0;

// LEDs: leuchten, wenn Spule 1 in Ordnung ist
int ledH1 = 14;
int ledH2 = 15;
int ledH3 = 16;

// LEDs: leuchten, wenn Spule 2 in Ordnung ist
int ledB1 = 17;
int ledB2 = 18;
int ledB3 = 19;

// Pins für Taster, Treiber und Potentiometer
int potPin = A0;
const int buttonPin = 2;
const int dirPin = 3;
const int stepPin = 4;
const int M2 = 5;
const int M1 = 6;
const int M0 = 7;
const int enPin = 8;

int potValue = 0;
int buttonState = 0;
int periode;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(potPin, INPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH);

  pinMode(ledH1, OUTPUT);
  pinMode(ledH2, OUTPUT);
  pinMode(ledH3, OUTPUT);

  pinMode(ledB1, OUTPUT);
  pinMode(ledB2, OUTPUT);
  pinMode(ledB3, OUTPUT);
}

void loop() {
  // Treiber aktivieren
  digitalWrite(enPin, LOW);

  // Schrittmodus: Vollschritt
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);

  // Spulenwiderstände über die Spannungsteiler berechnen
  sensorValue1 = analogRead(sensorPin1);
  Vs1 = (Ve1 * sensorValue1) / 1023;
  R1 = Rref1 * (1 / ((Ve1 / Vs1) - 1));

  sensorValue2 = analogRead(sensorPin2);
  Vs2 = (Ve2 * sensorValue2) / 1023;
  R2 = Rref2 * (1 / ((Ve2 / Vs2) - 1));

  // Spule 1: LEDs an, wenn der Widerstand im gültigen Bereich liegt
  if (R1 > 12000 || R1 < 0) {
    digitalWrite(ledH1, LOW);
    digitalWrite(ledH2, LOW);
    digitalWrite(ledH3, LOW);
  } else {
    digitalWrite(ledH1, HIGH);
    digitalWrite(ledH2, HIGH);
    digitalWrite(ledH3, HIGH);
  }

  // Spule 2
  if (R2 > 12000 || R2 < 0) {
    digitalWrite(ledB1, LOW);
    digitalWrite(ledB2, LOW);
    digitalWrite(ledB3, LOW);
  } else {
    digitalWrite(ledB1, HIGH);
    digitalWrite(ledB2, HIGH);
    digitalWrite(ledB3, HIGH);
  }

  // Taster lesen: Drehtest starten
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    // Eine Umdrehung im Uhrzeigersinn (48 Schritte)
    for (int i = 1; i <= 48; i++) {
      potValue = analogRead(potPin);
      periode = map(potValue, 0, 1024, 5, 100);   // Periode abhängig vom Potentiometer
      digitalWrite(dirPin, HIGH);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(periode);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(periode);

      lcd.setCursor(6, 0);
      lcd.print(round(1 / (periode * 0.001)));    // Frequenz anzeigen
      lcd.print(" Hz");
    }

    // Eine Umdrehung gegen den Uhrzeigersinn
    for (int i = 48; i <= 2 * 48; i++) {
      potValue = analogRead(potPin);
      periode = map(potValue, 0, 1024, 5, 100);
      digitalWrite(dirPin, LOW);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(periode);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(periode);

      lcd.setCursor(6, 0);
      lcd.print(round(1 / (periode * 0.001)));
      lcd.print(" Hz");
    }
  }
}
