#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int TRIG_PIN = 9;
const int ECHO_PIN = 8;
const int BTN_OUVERTURE = 2;
const int BTN_URGENCE = 3;
const int SERVO_PIN = 6;

const int DISTANCE_DETECTION = 30;
const int ANGLE_FERME = 10;
const int ANGLE_OUVERT = 170;
const int ATTENTE_OUVERTE = 10000;
const int DELAI_SERVO = 15;

enum EtatPorte {
  FERME,
  EN_OUVERTURE,
  OUVERT,
  EN_FERMETURE,
  URGENCE
};

Servo monServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

EtatPorte etat = FERME;

int angleActuel = ANGLE_FERME;
int angleSauvegarde = ANGLE_FERME;

unsigned long tempsOuvert = 0;
unsigned long dernierMouvServo = 0;

bool etatBtnOuverture = HIGH;
bool etatBtnOuverturePrev = HIGH;
bool etatBtnUrgence = HIGH;
bool etatBtnUrgencePrev = HIGH;

unsigned long dernierDebounceOuv = 0;
unsigned long dernierDebounceUrg = 0;

const int DEBOUNCE_DELAI = 50;

String derniereLigne0 = "";
String derniereLigne1 = "";

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BTN_OUVERTURE, INPUT_PULLUP);
  pinMode(BTN_URGENCE, INPUT_PULLUP);

  monServo.attach(SERVO_PIN);
  monServo.write(ANGLE_FERME);

  lcd.init();
  lcd.backlight();

  afficherLCD("Systeme pret", "Porte : FERMEE");

  Serial.println(" Porte automatique initialisee ");
}


void loop() {
  gererBoutons();
  etatFerme();
  etatEnOuverture();
  etatOuvert();
  etatEnFermeture();
}

void gererBoutons() {
  bool boutonOuvert = boutonPresse(BTN_OUVERTURE, etatBtnOuverture, etatBtnOuverturePrev, dernierDebounceOuv);
  bool boutonUrgence = boutonPresse(BTN_URGENCE, etatBtnUrgence, etatBtnUrgencePrev, dernierDebounceUrg);

  if (boutonUrgence) {
    if (etat != URGENCE) {
      angleSauvegarde = angleActuel;
      etat = URGENCE;
      monServo.write(angleActuel);
      afficherLCD("!!! URGENCE !!!", "Porte arretee");
    } else {
      etat = EN_FERMETURE;
      afficherLCD("Porte en cours", "de FERMETURE...");
    }
  }

  if (boutonOuvert && etat == FERME) {
    etat = EN_OUVERTURE;
    afficherLCD("Porte en cours", "d'OUVERTURE...");
  }
}

void etatFerme() {
  if (etat != FERME) return;

  long dist = lireDistance();

  Serial.print("Distance : ");
  Serial.print(dist);
  Serial.println(" cm");

  bool detectionClient = (dist > 0 && dist <= DISTANCE_DETECTION);

  if (detectionClient) {
    etat = EN_OUVERTURE;
    afficherLCD("Porte en cours", "d'OUVERTURE...");
    Serial.println("Ouverture declenchee");
  } else {
    afficherLCD("Systeme actif", "Porte : FERMEE");
  }
}

void etatEnOuverture() {
  if (etat != EN_OUVERTURE) return;

  mettreAJourServo();

  if (angleActuel >= ANGLE_OUVERT) {
    etat = OUVERT;
    tempsOuvert = millis();
    afficherLCD("Porte OUVERTE", "Attente 10 sec");
    Serial.println("Porte completement ouverte");
  }
}

void etatOuvert() {
  if (etat != OUVERT) return;

  if (millis() - tempsOuvert >= ATTENTE_OUVERTE) {
    etat = EN_FERMETURE;
    afficherLCD("Porte en cours", "de FERMETURE...");
    Serial.println("Fermeture automatique");
  }
}

void etatEnFermeture() {
  if (etat != EN_FERMETURE) return;

  bool btnOuvPresse = boutonPresse(BTN_OUVERTURE, etatBtnOuverture, etatBtnOuverturePrev, dernierDebounceOuv);

  if (btnOuvPresse) {
    etat = EN_OUVERTURE;
    Serial.println("Interruption fermeture : re-ouverture");
    return;
  }

  mettreAJourServo();

  if (angleActuel <= ANGLE_FERME) {
    etat = FERME;
    afficherLCD("Systeme actif", "Porte : FERMEE");
    Serial.println("Porte completement fermee");
  }
}

void mettreAJourServo() {
  if (millis() - dernierMouvServo < DELAI_SERVO) return;

  dernierMouvServo = millis();

  if (etat == EN_OUVERTURE && angleActuel < ANGLE_OUVERT) {
    angleActuel++;
    monServo.write(angleActuel);
  }
  else if (etat == EN_FERMETURE && angleActuel > ANGLE_FERME) {
    angleActuel--;
    monServo.write(angleActuel);
  }
}

long lireDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duree = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duree == 0) return -1;

  return (duree / 2) * 0.0343;
}

void afficherLCD(String ligne0, String ligne1) {
  if (ligne0 == derniereLigne0 && ligne1 == derniereLigne1) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(ligne0);

  lcd.setCursor(0, 1);
  lcd.print(ligne1);

  derniereLigne0 = ligne0;
  derniereLigne1 = ligne1;
}

bool boutonPresse(int pin, bool &etatActuel, bool &etatPrev, unsigned long &dernierDebounce) {
  bool lecture = digitalRead(pin);

  if (lecture != etatPrev) {
    dernierDebounce = millis();
  }

  etatPrev = lecture;

  if ((millis() - dernierDebounce) > DEBOUNCE_DELAI) {
    if (lecture != etatActuel) {
      etatActuel = lecture;

      if (etatActuel == LOW) {
        return true;
      }
    }
  }
  return false;
}