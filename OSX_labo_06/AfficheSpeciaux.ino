#include "AfficheSpeciaux.h"

Affiche affiche;

static void gererBouton() {
    affiche.urgence();
}

void setup() {
    affiche.begin();
    pinMode(PIN_URGENCE, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_URGENCE), gererBouton, FALLING);
}

void loop() {
    affiche.update();
}