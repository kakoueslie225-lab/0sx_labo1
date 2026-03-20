#include <LiquidCrystal_I2C.h>
#include <math.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int thermPin = A0;
const int joyX = A1;
const int joyY = A2;
const int buttonPin = 2;
const int ledPin = 8;
const String lastName="KAKOU";
float R1 = 10000;
float c1 = 1.009249522e-03;
float c2 = 2.378405444e-04;
float c3 = 2.019202697e-07;
float Vo, R2, logR2, T, Tc;
bool isCooling = false;
int xVal = 0;
int yVal = 0;
int altitude = 0;
int angle = 0;
bool page = false;
unsigned long lastSerial = 0;
unsigned long lastButton = 0;
bool lastButtonState = HIGH;
const int seuilMax=600;
const int seuilMin=400;
String NumeroAdmission = "2305238";

byte customChar[8] = {
  B11111,
  B00001,
  B01110,
  B00001,
  B11111,
  B10101,
  B11111,
  B00000
};

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  lcd.createChar(0, customChar);

  displayStart();
}

void loop() {
  temperature();
  cooling();

  readJoystick();
  DisplayAltitude();
  setAngle();

  buttonClic();
  newDisplay();

  serialDisplay();
}

void displayStart() {
  lcd.setCursor(0,0);
  lcd.print(lastName); 

  lcd.setCursor(0,1);
  lcd.write(0);

  lcd.setCursor(9,1);
  lcd.print("*****38");

  delay(3000);
  lcd.clear();
}

void temperature() {
  Vo = analogRead(thermPin);

  if (Vo == 0) return; 

  R2 = R1 * (1023.0 / Vo - 1.0);
  logR2 = log(R2);
  T = 1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2);
  Tc = T - 273.15;
}

void cooling() {
  if (Tc < 10) {
    isCooling = false;
  } 
  else if (Tc > 35) {
    isCooling = true;
  }

  digitalWrite(ledPin, isCooling ? HIGH : LOW);
}

void readJoystick() {
  xVal = analogRead(joyX);
  yVal = analogRead(joyY);
}

void DisplayAltitude() {
  unsigned long currentTime = millis();
  if (currentTime - lastButton >= 1000) {

    if (yVal > seuilMax && altitude < 200) {
      altitude++;
    }
    else if (yVal < seuilMin && altitude > 0) {
      altitude--;
    }

    lastButton = currentTime; 
  }
 
}

void setAngle() {
  angle = map(xVal, 0, 1023, -90, 90);
 
}

void buttonClic() {
  bool currentButton = digitalRead(buttonPin);

  if (currentButton == LOW && lastButtonState == HIGH && millis() - lastButton > 200) {
    page = !page;
    lastButton = millis();
  }

  lastButtonState = currentButton;
}

void newDisplay() {

  if (!page) {
    lcd.setCursor(0,0);

    if (yVal > seuilMax) lcd.print("UP   ");
    else if (yVal < seuilMin) lcd.print("DOWN ");
    else lcd.print("ALT:  ");

    lcd.setCursor(6,0);
    lcd.print(altitude);
    lcd.print("m  ");

    lcd.setCursor(0,1);
    lcd.print("DIR: ");
    lcd.print(angle);
    lcd.print((angle <= 0) ? " (D) " : " (G) ");
  }
  else {
    lcd.setCursor(0,0);
    lcd.print(Tc, 0);
    lcd.print(" C     ");

    lcd.setCursor(0,1);
    lcd.print("COOL: ");
    lcd.print(isCooling ? "  ON  " : "  OFF  ");
  }
}

void serialDisplay() {
  if (millis() - lastSerial >= 100) {
    lastSerial = millis();

    Serial.print("etd:");
    Serial.print(NumeroAdmission);
    Serial.print(",x:");
    Serial.print(xVal);
    Serial.print(",y:");
    Serial.print(yVal);
    Serial.print(",sys:");
    Serial.println(isCooling ? 1 : 0);
  }
}