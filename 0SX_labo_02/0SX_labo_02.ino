unsigned long currentTime = 0;
const int nbLed = 4;
const int ledPins[nbLed] = {7, 8, 9, 10};  
const int PIN_BUTTON = 2;             
const int potPin = A1;               
int potValue = 0;
int echelle = 20;
const int VALEURMIN = 0;
const int VALEURMAX=1023;
int pourcentageMax=100;
int valeur=0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(PIN_BUTTON, INPUT_PULLUP);
}

void loop() {
  currentTime= millis();
  allume();
  afficherBarre();
}

int estClic(unsigned long ct) {
  static unsigned long lastTime = 0;
  static int lastState = HIGH;
  const int rate = 50;
  int clic = 0;

  if (ct - lastTime < rate) {
    return clic; 
  }

  lastTime = ct;

  int state = digitalRead(PIN_BUTTON);

  if (state == LOW) {
    if (lastState == HIGH) {
      clic = 1; 
    }
  }

  lastState = state;

  return clic;
}
void allume(){
  potValue = analogRead(potPin); 
  valeur=map(potValue, 0, 1023, 0 ,20);
  int indexLed=0;

  if (valeur <=5) {
    indexLed = 0;
  }
  else if (valeur>5 && valeur <=10) {
    indexLed = 1;
  }
  else if (valeur>10 && valeur <= 15) {
    indexLed = 2;
  }
  else {
    indexLed = 3;
  }

  for (int i = 0; i < nbLed; i++) {
   if(i == indexLed ){
     digitalWrite(ledPins[i], HIGH);
   }
   else{
     digitalWrite(ledPins[i], LOW);
   }
  }
}

void afficherBarre() {
  int pourcentage = map(potValue, VALEURMIN, VALEURMAX, VALEURMIN,pourcentageMax);           
 
  if(estClic(currentTime)){
   Serial.print(pourcentage);
   Serial.print("%  [");
   for (int i = 0; i < echelle; i++){
     if (i<valeur){
       Serial.print("-");
     }
     else{
       Serial.print(".");
     }
   }
   Serial.println("]");
  }
}