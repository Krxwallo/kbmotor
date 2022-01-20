/**
   KBMotor
   =======
   by Ken & Björn
*/

// Sensor Pins
#define PIN_TRIGGER 12
#define PIN_ECHO    13
#define SENSOR_MAX_RANGE = 100; // in cm

// Motor Pins
// Left motor
#define m1A  6  // Pin 14 of L293
#define m1B  9  // Pin 10 of L293
// Right motor
#define m2A 10  // Pin  7 of L293
#define m2B  11 // Pin  2 of L293

// LED pins
#define ledG1 A2 // Left  green LED
#define ledR1 A3 // Left  red   LED
#define ledG2 A4 // Right green LED
#define ledR2 A5 // Right red   LED
#define ledBrightness 200

// Line-tracking sensors
#define sensorL A0 // Left sensor
#define sensorR A1 // Right sensor

#define VELOCITY_BUTTON 0
#define vHigh 200
#define vLow 100

// Line-tracking sensors
#define sensorL A0 // Left sensor
#define sensorR A1 // Right sensor

/**
 * Return true if the sensor reflects something (non-absorbing floor)
 * and false if the sensor doesn't reflect something (absorbing floor or no floor)
 */
boolean doesReflect(int sensorPin) { return analogRead(sensorPin) <= 100; }

unsigned long duration;
unsigned int distance;

boolean vButtonPressed = false;

boolean velocityButtonPressed() {
  vButtonPressed = digitalRead(VELOCITY_BUTTON) == LOW;
  return vButtonPressed;
}

// Get the velocity based on the button state
int v() {
  if (velocityButtonPressed()) {
    // Button pressed
    return vHigh;
  }
  return vLow;
}

// Helpers
void input(int pin)  { pinMode(pin, INPUT);   }
void output(int pin) { pinMode(pin, OUTPUT);  }
void high(int pin){ analogWrite(pin, v()); }
void low(int pin) { analogWrite(pin, 0);  }

void ledOn(int analogPin)  { analogWrite(analogPin, ledBrightness); }
void ledOff(int analogPin) { analogWrite(analogPin, LOW); }

void m1AHigh() { high(m1A); ledOn(ledG1);  }
void m2AHigh() { high(m2A); ledOn(ledG2);  }
void m1BHigh() { high(m1B); ledOn(ledR1);  }
void m2BHigh() { high(m2B); ledOn(ledR2);  }

void m1ALow() { low(m1A); ledOff(ledG1); }
void m2ALow() { low(m2A); ledOff(ledG2); }
void m1BLow() { low(m1B); ledOff(ledR1); }
void m2BLow() { low(m2B); ledOff(ledR2); }

//This will run only one time.
void setup() {
  Serial.begin(9600);
  //Set pins as outputs
  output(m1A);
  output(m1B);
  output(m2A);
  output(m2B);
  output(PIN_TRIGGER);
  input(PIN_ECHO);
  pinMode(VELOCITY_BUTTON, INPUT_PULLUP);

  pinMode(sensorL, INPUT);
}


boolean right = true;
void loop() {
  /*
   * Forward until it finds an obstacle; then right until it no longer sees an obstacle
   */
  forwardUntil(70);
  if (sensorDistance() < 30) backwardsUntil(35);
  if (right) {
    rightUntilNot(70);
    right = false;
  }
  else {
    leftUntilNot(70);
    right = true;
  }
}

void turnTest1() {
  /*
     1. Turn hard right
     2. Turn right
     3. Turn hard left
     4. Turn left
  */
  r(true);
  delay(5000);
  r(false);
  delay(5000);
  l(true);
  delay(5000);
  l(false);
  delay(5000);
}

// Forwards
void f() {
  st();
  m1AHigh();
  m2AHigh();
}

// Backwards
void b() {
  st();
  m1BHigh();
  m2BHigh();
}

// Turn right on the spot
void r(boolean hard) {
  st();
  m1AHigh();
  if (hard) {
    m2BHigh();
  }
}

// Turn left on the spot
void l(boolean hard) {
  st();
  m2AHigh();
  if (hard) {
    m1BHigh();
  }
}

void st() {
  m1ALow();
  m1BLow();
  m2ALow();
  m2BLow();
}

/*
 * Go forward until sth is in range of the sensor.
 */
void forwardUntil(int sensorRange) {
  while (true) {
    boolean check = checkSensor(sensorRange);
    f();
    if (check) break;
    delay(300);
  }
  
  st();
}

/*
 * Go backwards until sth is out of range of the sensor.
 */
void backwardsUntil(int sensorRange) {
  while (true) {
    b();
    if (sensorDistance() > sensorRange) break;
    delay(300);
  }
  
  st();
}

/*
 * Go right until nothing is in range of the sensor.
 */
void rightUntilNot(int sensorRange) {
  while (true) {
    boolean check = checkSensor(sensorRange);
    r(true);
    if (!check) break;
    delay(300);
  }
  st();
}

/*
 * Go left until nothing is in range of the sensor.
 */
void leftUntilNot(int sensorRange) {
  while (true) {
    boolean check = checkSensor(sensorRange);
    l(true);
    if (!check) break;
    delay(300);
  }
  st();
}

int sensorDistance() {
  digitalWrite(PIN_TRIGGER, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);

  duration = pulseIn(PIN_ECHO, HIGH);
  return duration / 58;
}


/**
   Return true if the sensor detected object in the given sensorRange
*/
boolean checkSensor(int sensorRange) {
  return sensorDistance() < sensorRange;
}
