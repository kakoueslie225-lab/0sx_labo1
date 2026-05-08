#ifndef GESTIONNAIRE_LCD_H
#define GESTIONNAIRE_LCD_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

enum class Etat : uint8_t { NORMAL, RABAIS, ERREUR, FERMER, URGENCE };

class GestionnaireLCD {
public:
    GestionnaireLCD(uint8_t adresse, uint8_t cols, uint8_t rows);
    void begin();
    void afficher(Etat etat);

private:
    LiquidCrystal_I2C _lcd;
    Etat _dernierEtat;
};

#endif