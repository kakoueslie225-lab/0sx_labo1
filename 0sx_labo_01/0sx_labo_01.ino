
int led= LED_BUILTIN;
int position=0;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(led, OUTPUT);
}


void eteint_allume(){
  Serial.println("Etat: Allume - 2305238");
  digitalWrite(led,LOW);
  delay(300);
  digitalWrite(led,HIGH);
  delay(2000);
  digitalWrite(led,LOW);
  delay(1000);
}

void variation(){
  delay(3000);
   Serial.println("Etat : Varie – 2305238");

  for (int i=0; i<255;i++){
     analogWrite(led, i); 
       delay(8); 

  }
  }
void clignotement(){
   Serial.println("Etat: CLignotement - 2305238");

   for (int i = 0; i < 2; i++) {
    digitalWrite(led, LOW);
    delay(350);
    digitalWrite(led, HIGH);
    delay(350);
   }
   
}
void loop() {
  // put your main code here, to run repeatedly:
switch(position){
   case 0: eteint_allume();position++;break;
   case 1: variation(); position++;break;
   case 2: clignotement();position=0; break;
  
}
}
