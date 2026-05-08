#include "GestionnaireMatrice.h"
#include "AfficheSpeciaux.h"

static const uint8_t SMILEY[8] = {
    0b00111100, 0b01000010, 0b10100101, 0b10000001,
    0b10100101, 0b10011001, 0b01000010, 0b00111100
};
static const uint8_t CROIX[8] = {
    0b10000001, 0b01000010, 0b00100100, 0b00011000,
    0b00011000, 0b00100100, 0b01000010, 0b10000001
};
static const uint8_t FONT[][5] = {
    { 0x62, 0x91, 0x91, 0x91, 0x4E },
    { 0xFF, 0x88, 0x88, 0x88, 0x70 },
    { 0xFF, 0x89, 0x89, 0x89, 0x81 },
    { 0x7E, 0x81, 0x81, 0x81, 0x42 },
    { 0x00, 0x81, 0xFF, 0x81, 0x00 },
    { 0x7F, 0x88, 0x88, 0x88, 0x7F },
    { 0xFF, 0x01, 0x01, 0x01, 0x01 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static const int NB_CHARS = 8;
static const int COL_W    = 6;

GestionnaireMatrice::GestionnaireMatrice(uint8_t din, uint8_t clk, uint8_t cs)
    : _matrice(din, clk, cs, 1) {}

void GestionnaireMatrice::begin() {
    _matrice.shutdown(0, false);
    _matrice.setIntensity(0, 5);
    _matrice.clearDisplay(0);
}

void GestionnaireMatrice::afficherImage(const uint8_t img[8]) {
    for (int r = 0; r < 8; r++)
        _matrice.setRow(0, r, img[r]);
}

void GestionnaireMatrice::traiterEtat(Etat etat, int& scrollPos,
                                       unsigned long& dernierScroll,
                                       bool& blinkOn, unsigned long& dernierBlink) {
    unsigned long now = millis();
    switch (etat) {
        case Etat::NORMAL:
            afficherImage(SMILEY);
            break;

        case Etat::RABAIS:
            if (now - dernierScroll < DELAI_DEFILE) break;
            dernierScroll = now;
            for (int col = 0; col < 8; col++) {
                int src     = (scrollPos + col) % (NB_CHARS * COL_W);
                int idxChar = src / COL_W;
                int idxCol  = src % COL_W;
                int donnee  = (idxCol < 5) ? FONT[idxChar][idxCol] : 0x00;
                _matrice.setColumn(0, col, donnee);
            }
            scrollPos = (scrollPos + 1) % (NB_CHARS * COL_W);
            break;

        case Etat::ERREUR:
            if (now - dernierBlink < DELAI_BLINK) break;
            dernierBlink = now;
            blinkOn = !blinkOn;
            for (int r = 0; r < 8; r++)
                _matrice.setRow(0, r, blinkOn ? 0xFF : 0x00);
            break;

        case Etat::FERMER:
        case Etat::URGENCE:
            break;
    }
}

void GestionnaireMatrice::clear() {
    _matrice.clearDisplay(0);
}