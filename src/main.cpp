#include <Arduino.h>
const int topeniInput = 2; // Pin tlačítka
const int vodaInput = 3;
const int resetInput = 7;   // Pin resetovacího tlačítka
const int teplota30 = 8;    // Pin první LED
const int teplota60 = 9;    // Pin druhé LED
const int teplota90 = 10;   // Pin třetí LED
const int errorLedPin = 13; // Pin chybové LED
const int hladina50 = 11;
const int hladina100 = 12;

bool buttonHeldTopeni = false;
bool buttonHeldVoda = false;           // Stav tlačítka
bool errorState = false;               // Stav chyby
unsigned long previousTimeTopeni = 0;  // Uložený čas pro řízení logiky
unsigned long holdStartTimeTopeni = 0; // Čas, kdy bylo tlačítko drženo po třetí LED
int ledStateTopeni = 0;
unsigned long previousTimeVoda = 0;  // Uložený čas pro řízení logiky
unsigned long holdStartTimeVoda = 0; // Čas, kdy bylo tlačítko drženo po druhé LED
int ledStateVoda = 0;                // Stav (0: vše zhasnuto, 1-3: rozsvícené, -1 až -3: zhasínání)

void voda(){
  // Číst stav tlačítek
  bool buttonPressed = digitalRead(vodaInput) == HIGH;
  bool resetPressed = digitalRead(resetInput) == HIGH;

  // Resetování při stisknutí resetovacího tlačítka
  if (resetPressed) {
    errorState = false; // Zrušení stavu chyby
    ledStateVoda = 0;       // Resetování stavu LED
    buttonHeldVoda = false; // Resetování stavu tlačítka
    holdStartTimeVoda = 0;

    // Vypnutí všech LED
    digitalWrite(hladina50, LOW);
    digitalWrite(hladina100, LOW);
    digitalWrite(errorLedPin, LOW);
    return; // Ukončit smyčku, aby se další logika neprováděla
  }

  // Pokud je chybový stav aktivní, nedělat nic dalšího
  if (errorState) {
    return;
  }

  // Kontrola tlačítka
  if (buttonPressed) {
    if (!buttonHeldVoda) { // Pokud tlačítko nebylo dříve stisknuto
      buttonHeldVoda = true;
      ledStateVoda = 1; // Začněte od první LED
      previousTimeVoda = millis(); // Nastavte čas
    }
  } else {
    buttonHeldVoda = false; // Resetovat držení tlačítka, pokud bylo puštěno
  }

  // Časování pomocí millis()
  unsigned long currentTimeVoda = millis();

  // Logika pro rozsvěcování LED
  if (buttonHeldVoda) {
    if (ledStateVoda == 1 && currentTimeVoda - previousTimeVoda >= 5000) {
      digitalWrite(hladina100, HIGH); // Rozsvítit druhou LED
      ledStateVoda = 2;
      previousTimeVoda = currentTimeVoda;
    } else if (ledStateVoda == 1) {
      digitalWrite(hladina50, HIGH); // Rozsvítit první LED
    }

    // Kontrola dlouhého držení tlačítka po rozsvícení druhé LED
    if (ledStateVoda == 2) {
      if (holdStartTimeVoda == 0) { // Zahájit měření držení tlačítka
        holdStartTimeVoda = currentTimeVoda;
      } else if (currentTimeVoda - holdStartTimeVoda >= 2000) {
        digitalWrite(errorLedPin, HIGH); // Rozsvítit chybovou LED
        errorState = true;               // Nastavit chybový stav
        // Ihned vypnout obě LED
      digitalWrite(hladina50, LOW);
    digitalWrite(hladina100, LOW);
      }
    }
  }

  // Reset časování držení tlačítka, pokud není dosažena druhá LED
  if (ledStateVoda != 2) {
    holdStartTimeVoda = 0;
  }

}
void topeni()
{
  // Číst stav tlačítek
  bool buttonPressed = digitalRead(topeniInput) == HIGH;
  bool resetPressed = digitalRead(resetInput) == HIGH;

  // Resetování při stisknutí resetovacího tlačítka
  if (resetPressed)
  {
    errorState = false;       // Zrušení stavu chyby
    ledStateTopeni = 0;       // Resetování stavu LED
    buttonHeldTopeni = false; // Resetování stavu tlačítka
    holdStartTimeTopeni = 0;

    // Vypnutí všech LED
    digitalWrite(teplota30, LOW);
    digitalWrite(teplota60, LOW);
    digitalWrite(teplota90, LOW);
    digitalWrite(errorLedPin, LOW);
    return; // Ukončit smyčku, aby se další logika neprováděla
  }

  // Pokud je chybový stav aktivní, nedělat nic dalšího
  if (errorState)
  {
    return;
  }

  // Kontrola tlačítka
  if (buttonPressed)
  {
    if (!buttonHeldTopeni)
    { // Pokud tlačítko nebylo dříve stisknuto
      buttonHeldTopeni = true;
      ledStateTopeni = 1;            // Začněte od první LED
      previousTimeTopeni = millis(); // Nastavte čas
    }
  }
  else
  {
    if (buttonHeldTopeni)
    { // Pokud bylo tlačítko puštěno
      buttonHeldTopeni = false;
      ledStateTopeni = -3; // Zahajte zhasínání od poslední LED
      previousTimeTopeni = millis();
    }
  }

  // Časování pomocí millis()
  unsigned long currentTimeTopeni = millis();

  // Logika pro rozsvěcování
  if (buttonHeldTopeni)
  {
    if (ledStateTopeni == 1 && currentTimeTopeni - previousTimeTopeni >= 3000)
    {
      digitalWrite(teplota60, HIGH); // Rozsvítit druhou LED
      ledStateTopeni = 2;
      previousTimeTopeni = currentTimeTopeni;
    }
    else if (ledStateTopeni == 2 && currentTimeTopeni - previousTimeTopeni >= 3000)
    {
      digitalWrite(teplota90, HIGH); // Rozsvítit třetí LED
      ledStateTopeni = 3;
      previousTimeTopeni = currentTimeTopeni;
    }
    else if (ledStateTopeni == 1)
    {
      digitalWrite(teplota30, HIGH); // Rozsvítit první LED
    }

    // Kontrola dlouhého držení tlačítka po rozsvícení třetí LED
    if (ledStateTopeni == 3)
    {
      if (holdStartTimeTopeni == 0)
      { // Zahájit měření držení tlačítka
        holdStartTimeTopeni = currentTimeTopeni;
      }
      else if (currentTimeTopeni - holdStartTimeTopeni >= 1000)
      {
        digitalWrite(errorLedPin, HIGH); // Rozsvítit chybovou LED
        errorState = true;               // Nastavit chybový stav
        // Ihned vypnout všechny tři LED
        digitalWrite(teplota30, LOW);
        digitalWrite(teplota60, LOW);
        digitalWrite(teplota90, LOW);
      }
    }
  }

  // Reset časování držení tlačítka, pokud nebyla třetí LED
  if (ledStateTopeni != 3)
  {
    holdStartTimeTopeni = 0;
  }

  // Logika pro zhasínání
  if (!buttonHeldTopeni)
  {
    if (ledStateTopeni == -3 && currentTimeTopeni - previousTimeTopeni >= 2000)
    {
      digitalWrite(teplota90, LOW); // Zhasnout třetí LED
      ledStateTopeni = -2;
      previousTimeTopeni = currentTimeTopeni;
    }
    else if (ledStateTopeni == -2 && currentTimeTopeni - previousTimeTopeni >= 2000)
    {
      digitalWrite(teplota60, LOW); // Zhasnout druhou LED
      ledStateTopeni = -1;
      previousTimeTopeni = currentTimeTopeni;
    }
    else if (ledStateTopeni == -1 && currentTimeTopeni - previousTimeTopeni >= 2000)
    {
      digitalWrite(teplota30, LOW); // Zhasnout první LED
      ledStateTopeni = 0;
    }
  }
}
void setup()
{
  pinMode(topeniInput, INPUT); // Tlačítko jako vstup (s pull-up rezistorem)
  pinMode(resetInput, INPUT);  // Resetovací tlačítko jako vstup (s pull-up rezistorem)
  pinMode(teplota30, OUTPUT);  // LED jako výstupy
  pinMode(teplota60, OUTPUT);
  pinMode(teplota90, OUTPUT);
  pinMode(errorLedPin, OUTPUT);

  pinMode(vodaInput, INPUT); // Tlačítko jako vstup (s pull-up rezistorem)
  pinMode(hladina50, OUTPUT);  // LED jako výstupy
  pinMode(hladina100, OUTPUT);
  pinMode(errorLedPin, OUTPUT);

  // Všechny LED zhasnuté na začátku
  digitalWrite(teplota30, LOW);
  digitalWrite(teplota60, LOW);
  digitalWrite(teplota90, LOW);
  digitalWrite(errorLedPin, LOW);
   digitalWrite(hladina100, LOW);
  digitalWrite(hladina50, LOW);
}

void loop()
{
  topeni();
  voda();
}
