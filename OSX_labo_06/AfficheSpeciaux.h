#ifndef AFFICHE_SPECIAUX_H
#define AFFICHE_SPECIAUX_H

#include <Arduino.h>
#include "Etat.h"
#include "GestionnaireLCD.h"
#include "GestionnaireMatrice.h"

#define PIN_IR          8
#define PIN_URGENCE     3
#define PIN_DIN         34
#define PIN_CLK         30
#define PIN_CS          32
#define LCD_ADRESSE     0x27

#define DELAI_DEFILE    150
#define DELAI_BLINK     400
#define DELAI_DEBOUNCE  200

constexpr uint8_t btn_RABAIS = 0x0C;
constexpr uint8_t btn_NORMAL = 0x18;
constexpr uint8_t btn_ERREUR = 0x5E;
constexpr uint8_t btn_FERMER = 0x08;

class Affiche {
public:
    Affiche();
    void begin();
    void update();
    void setEtat(Etat nouvel);
    void urgence();

private:
    Etat _etat;
    Etat _etatAvantUrgence;
    volatile bool _urgenceFlag;
    int _scrollPos;
    unsigned long _dernierScroll;
    bool _blinkOn;
    unsigned long _dernierBlink;

    GestionnaireLCD     _lcd;
    GestionnaireMatrice _matrice;

    void lireEntrees();
    void traiterUrgence();
};

#endif