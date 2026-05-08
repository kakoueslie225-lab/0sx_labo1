#ifndef GESTIONNAIRE_MATRICE_H
#define GESTIONNAIRE_MATRICE_H

#include <Arduino.h>
#include <LedControl.h>
#include "Etat.h"

class GestionnaireMatrice {
public:
    GestionnaireMatrice(uint8_t din, uint8_t clk, uint8_t cs);
    void begin();
    void afficherImage(const uint8_t img[8]);
    void traiterEtat(Etat etat, int& scrollPos, unsigned long& dernierScroll,
                     bool& blinkOn, unsigned long& dernierBlink);
    void clear();

private:
    LedControl _matrice;
};

#endif