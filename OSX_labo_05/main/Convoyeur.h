#ifndef CONVOYEUR_H
#define CONVOYEUR_H

#include <Arduino.h>

class Convoyeur {
public:
  Convoyeur(int pinIn1, int pinIn2,
            int pinEnable,
            int pinLed1, int pinLed2);

  void begin();

  void avancer();
  void reculer();
  void arreter();
  void ajusterVitesse(int deltaX); 

  bool estActif()   const;
  bool estAvant()   const;
  int  getVitesse() const;

  void setLedUrgence(bool allumee);

private:
  int _pinIn1;
  int _pinIn2;
  int _pinEnable;
  int _pinLed1;
  int _pinLed2;

  bool _actif;
  bool _avant;
  int  _vitesse;

  void majLedStatut();
  void appliquerVitesse();
};

#endif