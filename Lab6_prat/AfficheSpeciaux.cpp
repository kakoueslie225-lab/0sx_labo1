#include "AfficheSpeciaux.h"
#include <IRremote.hpp>

static const uint8_t CROIX[8] = {
    0b10000001, 0b01000010, 0b00100100, 0b00011000,
    0b00011000, 0b00100100, 0b01000010, 0b10000001
};

Affiche::Affiche()
    : _etat(Etat::NORMAL),
      _etatAvantUrgence(Etat::NORMAL),
      _urgenceFlag(false),
      _scrollPos(0),
      _dernierScroll(0),
      _blinkOn(false),
      _dernierBlink(0),
      _lcd(LCD_ADRESSE, 16, 2),
      _matrice(PIN_DIN, PIN_CLK, PIN_CS) {}

void Affiche::begin() {
    _lcd.begin();
    _matrice.begin();
    IrReceiver.begin(PIN_IR, DISABLE_LED_FEEDBACK);
    Serial.begin(9600);
    Serial.println(F("1=Rabais 2=Normal 3=Erreur 4=Ferme"));
}

void Affiche::update() {
    if (_urgenceFlag) {
        _urgenceFlag = false;
        traiterUrgence();
        return;
    }
    if (_etat == Etat::URGENCE) return;

    lireEntrees();
    _matrice.traiterEtat(_etat, _scrollPos, _dernierScroll, _blinkOn, _dernierBlink);
}

void Affiche::setEtat(Etat nouvel) {
    if (nouvel == _etat) return;
    _etat = nouvel;
    _scrollPos = 0;
    _blinkOn = false;
    _matrice.clear();
    _lcd.afficher(_etat);
}

void Affiche::urgence() { _urgenceFlag = true; }

void Affiche::lireEntrees() {
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.decodedRawData != 0) {
            int commande = IrReceiver.decodedIRData.command;
            Serial.print(F("[IR] 0x")); Serial.println(commande, HEX);
            switch (commande) {
                case btn_RABAIS: setEtat(Etat::RABAIS); break;
                case btn_NORMAL: setEtat(Etat::NORMAL); break;
                case btn_ERREUR: setEtat(Etat::ERREUR); break;
                case btn_FERMER: setEtat(Etat::FERMER); break;
            }
        }
        IrReceiver.resume();
    }

    if (Serial.available()) {
        switch (Serial.read()) {
            case '1': setEtat(Etat::RABAIS); break;
            case '2': setEtat(Etat::NORMAL); break;
            case '3': setEtat(Etat::ERREUR); break;
            case '4': setEtat(Etat::FERMER); break;
        }
    }
}

void Affiche::traiterUrgence() {
    if (_etat != Etat::URGENCE) {
        _etatAvantUrgence = _etat;
        _etat = Etat::URGENCE;
        _matrice.afficherImage(CROIX);
        _lcd.afficher(_etat);
        Serial.println(F("Mode arret d'urgence."));
    } else {
        _matrice.clear();
        setEtat(_etatAvantUrgence);
        Serial.println(F("Systeme redemarré."));
    }
}