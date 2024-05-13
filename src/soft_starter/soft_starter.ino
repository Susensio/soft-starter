const int LED0 = PB1;
const int LED1 = PB0;
const int LED2 = PB4;

const int POT1 = A1;
const int POT2 = A3;

class Smoother {
  private:
    int pin;
    byte i;
    int readings[10];
    int previous;
  public:
    Smoother(int inputPin) {
      pin = inputPin;
      i = 0;
      previous = 0;
      for (byte i = 0; i < 10; i++) {
        readings[i] = 0;
      }
    }
    void init();
    int update();
};

void Smoother::init() {
  int value = analogRead(pin);
  for (byte i = 0; i < 10; i++) {
    readings[i] = value;
  }
  previous = value;
  i = 0;
}

int Smoother::update() {
  int value = analogRead(pin);
  readings[i] = value;
  i = (i + 1) % 10;

  int sum = 0;
  for (byte i = 0; i < 10; i++) {
    sum += readings[i];
  }
  int average = sum / 10;
  // hysterisis
  if (abs(average - previous) > 1) {
    previous = average;
  }
  return previous;
}

Smoother sp1(POT1);
Smoother sp2(POT2);

unsigned long startMillis;
const unsigned long RAMP_MILLIS = 700;


const byte gammaCurve[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


void setup() {
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  sp1.init();
  sp2.init();

  startMillis = millis();
}


void loop() {
  unsigned long currentMillis;
  unsigned long elapsedMillis;
  byte fadeValue;

  do {
    currentMillis = millis();
    elapsedMillis = currentMillis - startMillis;
    fadeValue = map(elapsedMillis, 0, RAMP_MILLIS, 0, 255);
    updateLeds(fadeValue);
  } while (elapsedMillis < RAMP_MILLIS);

  fadeValue = 255;

  while (true) {
    updateLeds(fadeValue);
    delay(100);
  }
}

byte readPot(Smoother &sp) {
  int potValue = sp.update();
  byte value = map(potValue, 0, 1023, 0, 255);
  return gammaCurve[value];
}


void updateLeds(byte fadeValue) {
  byte potValue;
  byte ledValue;

  // LED0 tops at max value
  analogWrite(LED0, fadeValue);

  potValue = readPot(sp1);
  ledValue = map(fadeValue, 0, 255, 0, potValue);
  analogWrite(LED1, ledValue);

  potValue = readPot(sp2);
  ledValue = map(fadeValue, 0, 255, 0, potValue);
  analogWrite(LED2, ledValue);
}
