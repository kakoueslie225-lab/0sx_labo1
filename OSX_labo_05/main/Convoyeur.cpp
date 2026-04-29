#include "Convoyeur.h"
#include <Arduino.h>

const int VITESSE_INITIALE = 0;
const int PAS_VITESSE = 1;
const int VITESSE_MIN = 80;
const int VITESSE_MAX = 255;
const int JOYSTICK_CENTRE = 512;
Convoyeur::Convoyeur(int pinIn1, int pinIn2,
                     int pinEnable,
                     int pinLed1, int pinLed2)
  : _pinIn1(pinIn1), _pinIn2(pinIn2),
    _pinEnable(pinEnable),
    _pinLed1(pinLed1), _pinLed2(pinLed2),
    _actif(false), _avant(true), _vitesse(VITESSE_INITIALE)
{}

void Convoyeur::begin() {
  pinMode(_pinIn1, OUTPUT);
  pinMode(_pinIn2, OUTPUT);
  pinMode(_pinEnable, OUTPUT);
  pinMode(_pinLed1, OUTPUT);
  pinMode(_pinLed2, OUTPUT);

  arreter();
  setLedUrgence(false);
}

void Convoyeur::ajusterVitesse(int deltaX) {
  int pas = map(abs(deltaX), 0, JOYSTICK_CENTRE, 5, 40);
  pas = constrain(pas, 5, 40);

  if (deltaX > 0) {
    _vitesse = constrain(_vitesse + pas, VITESSE_MIN, VITESSE_MAX);
  } else {
    _vitesse = constrain(_vitesse - pas, VITESSE_MIN, VITESSE_MAX);
  }

  Serial.print("VITESSE: "); Serial.println(_vitesse);
  appliquerVitesse();
}
void Convoyeur::appliquerVitesse() {
  if (_actif) {
    analogWrite(_pinEnable, _vitesse);
  }
}
void Convoyeur::avancer() {
  _actif = true;
  _avant = true;

   if (_vitesse < VITESSE_MIN) _vitesse = VITESSE_MIN;
  digitalWrite(_pinIn1, HIGH);
  digitalWrite(_pinIn2, LOW);
  analogWrite(_pinEnable, _vitesse);

  majLedStatut();
}

void Convoyeur::reculer() {
  _actif = true;
  _avant = false;

  if (_vitesse == 0) _vitesse = VITESSE_MIN;

  digitalWrite(_pinIn1, LOW);
  digitalWrite(_pinIn2, HIGH);
  analogWrite(_pinEnable, _vitesse);

  majLedStatut();
}

void Convoyeur::arreter() {
  _actif = false;

  analogWrite(_pinEnable, 0);
  digitalWrite(_pinIn1, LOW);
  digitalWrite(_pinIn2, LOW);

  majLedStatut();
}

bool Convoyeur::estActif() const {
  return _actif;
}

bool Convoyeur::estAvant() const {
  return _avant;
}

int Convoyeur::getVitesse() const {
  return _vitesse;
}

void Convoyeur::setLedUrgence(bool allumee) {
  digitalWrite(_pinLed2, allumee ? HIGH : LOW);
}

void Convoyeur::majLedStatut() {
  digitalWrite(_pinLed1, _actif ? HIGH : LOW);
}