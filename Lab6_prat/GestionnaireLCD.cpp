#include "GestionnaireLCD.h"

static const String NOMS_ETAT[] = {
    "Normal", "Rabais", "Erreur", "Ferme", "ARRET URGENCE"
};

GestionnaireLCD::GestionnaireLCD(uint8_t adresse, uint8_t cols, uint8_t rows)
    : _lcd(adresse, cols, rows),
      _dernierEtat(Etat::NORMAL) {}

void GestionnaireLCD::begin() {
    _lcd.init();
    _lcd.backlight();
    _lcd.clear();
    _lcd.setCursor(0, 0); _lcd.print(F("Mode:"));
    _lcd.setCursor(0, 1); _lcd.print(F("Normal"));
}

void GestionnaireLCD::afficher(Etat etat) {
    if (etat == _dernierEtat) return;
    _dernierEtat = etat;
    _lcd.clear();
    _lcd.setCursor(0, 0); _lcd.print(F("Mode:"));
    _lcd.setCursor(0, 1); _lcd.print(NOMS_ETAT[(int)etat]);
}