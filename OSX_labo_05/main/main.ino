//2305238
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Convoyeur.h"

const int PIN_JOYSTICK_Y = A0;
const int PIN_JOYSTICK_X = A1;
const int PIN_BOUTON_URG = A2;
const int PIN_MOTEUR_IN1 = 44;
const int PIN_MOTEUR_IN2 = 45;
const int PIN_MOTEUR_EN  = 6;
const int PIN_LED1 = 7;
const int PIN_LED2 = 8;
const int JOYSTICK_CENTRE = 512;
const int JOYSTICK_ZONE_MORTE = 150;

const unsigned long DELAI_DEFILEMENT = 2000;
const unsigned long DEBOUNCE_BTN = 200;
const unsigned long DELAI_VITESSE = 80;
unsigned long dernierAjustVitesse = 0;

enum Etat {
  ETAT_ARRET,
  ETAT_MARCHE,
  ETAT_URGENCE
};

LiquidCrystal_I2C lcd(0x27, 16, 2);
Convoyeur convoyeur(PIN_MOTEUR_IN1, PIN_MOTEUR_IN2,
                    PIN_MOTEUR_EN,
                    PIN_LED1, PIN_LED2);


Etat etatActuel = ETAT_ARRET;
int pageLCD = 0;
unsigned long dernierDefilement = 0;
unsigned long dernierBouton = 0;
bool boutonPrecedent = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BOUTON_URG, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  convoyeur.begin();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Convoyeur V1  ");
  lcd.setCursor(0, 1);
  lcd.print("   Pret...      ");
  delay(1500);
  lcd.clear();
}

void loop() {
  gererBoutonUrgence();

  switch (etatActuel) {
    case ETAT_ARRET:   traiterArret();   break;
    case ETAT_MARCHE:  traiterMarche();  break;
    case ETAT_URGENCE: traiterUrgence(); break;
  }

  afficherLCD();
}

void gererBoutonUrgence() {
  bool boutonActuel = digitalRead(PIN_BOUTON_URG);
  unsigned long now = millis();

  if (boutonPrecedent == HIGH && boutonActuel == LOW &&
      (now - dernierBouton) > DEBOUNCE_BTN) {
    dernierBouton = now;

    if (etatActuel != ETAT_URGENCE) {
      entrerUrgence();
    } else {
      quitterUrgence();
    }
  }

  boutonPrecedent = boutonActuel;
}

void entrerUrgence() {
  convoyeur.arreter();
  convoyeur.setLedUrgence(true);
  etatActuel = ETAT_URGENCE;
  pageLCD    = 0;
  afficherUrgenceImmediatement();
}

void quitterUrgence() {
  convoyeur.setLedUrgence(false);
  etatActuel = ETAT_ARRET;
  pageLCD = 0;
  lcd.clear();
}

void traiterArret() {
  int joystickY = analogRead(PIN_JOYSTICK_Y);
  int deltaY = joystickY - JOYSTICK_CENTRE;

  if (abs(deltaY) > JOYSTICK_ZONE_MORTE) {
    etatActuel = ETAT_MARCHE;
    appliquerDirection(deltaY);
  }
}

void traiterMarche() {
  int joystickY = analogRead(PIN_JOYSTICK_Y);
  int joystickX = analogRead(PIN_JOYSTICK_X);
  int deltaY = joystickY - JOYSTICK_CENTRE;
  int deltaX = joystickX - JOYSTICK_CENTRE;

  unsigned long now = millis();
  if (abs(deltaX) > JOYSTICK_ZONE_MORTE &&
      (now - dernierAjustVitesse) >= DELAI_VITESSE) {
    dernierAjustVitesse = now;
    convoyeur.ajusterVitesse(deltaX);
  }

  if (abs(deltaY) <= JOYSTICK_ZONE_MORTE) {
    convoyeur.arreter();
    etatActuel = ETAT_ARRET;
  } else {
    appliquerDirection(deltaY);
  }
}

void traiterUrgence() {
  //y a rien qui se passe 
}

void appliquerDirection(int deltaY) {
  if (deltaY > 0) {
    convoyeur.avancer();
  } else {
    convoyeur.reculer();
  }
}

void afficherLCD() {
  if (etatActuel == ETAT_URGENCE) return;

  unsigned long now = millis();
  if (now - dernierDefilement >= DELAI_DEFILEMENT) {
    dernierDefilement = now;
    pageLCD = (pageLCD + 1) % 2;
    lcd.clear();
  }

  afficherPageCourante();
}

void afficherPageCourante() {
  if (pageLCD == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Actif: ");
    lcd.print(convoyeur.estActif() ? "OUI  " : "NON  ");

    lcd.setCursor(0, 1);
    lcd.print("Sens: ");
    if (!convoyeur.estActif()) {
      lcd.print("------");
    } else {
      lcd.print(convoyeur.estAvant() ? "AVANT     " : "ARRIERE   ");
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Vitesse:        ");
    lcd.setCursor(0, 1);
    lcd.print(convoyeur.getVitesse());
 
  }
}

void afficherUrgenceImmediatement() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("!! URGENCE !!");
  lcd.setCursor(2, 1);
  lcd.print("Appui = reset");
}