int pinBouton = 2; // Pin du bouton
int ledPin=9;
void setup() {
  Serial.begin(9600);
  pinMode(pinBouton, INPUT_PULLUP);
}

void loop() {
  static int etatPrecedent = HIGH; // État initial cohérent avec INPUT_PULLUP
  static int etat = HIGH; // État stable du bouton
  const int delai = 50; // Délai anti-rebond en ms
  static unsigned long dernierChangement = 0; // Dernier changement d'état

  int etatPresent = digitalRead(pinBouton); // Lecture de l'état du capteur

  if (etatPresent != etatPrecedent) { // Si l'état change
    dernierChangement = millis(); // Mise à jour du temps
    if(etatPresent==1){
     digitalWrite (ledPin,HIGH);
    }
    else  {
    digitalWrite (ledPin,LOW);
    }
  }

  if ((millis() - dernierChangement) > delai && etatPresent != etat) { 
    etat = etatPresent; // Mise à jour de l’état stable
    Serial.println(etat); // Affichage
  }

  etatPrecedent = etatPresent; // Mise à jour pour la prochaine itération
}