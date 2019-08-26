/*
 * ArduGov - A standalone Arduino Uno based engine governor.
 * 
 * Pins assignments:
 * 7 - Governor on/off switch.
 * 8 - Tach pulse input
 * 9 - Servo output
 * 
 * Onboard LED indicates active PID control.
 * 
 */

// Target RPM value
#define RPM_SETPOINT 3600
#define BANGBANG_LIMIT 0 

// Degrees of rotation per tach pulse
#define DEGREES_PER_PULSE 20

// Servo limits in uS
#define SERVO_MIN 1135
#define SERVO_MAX 1735

// PID settings
#define KP 0.50
#define KI 0.01
#define KD 0.02

// Tach pin assignment is static due to FreqMeasure limitations
#define ACTIVATE_PIN 7
#define SERVO_PIN 9

#include <FreqMeasure.h>
#include <AutoPID.h>
#include <ServoTimer2.h>

double currentRPM = 0;
double targetRPM = RPM_SETPOINT;
double servoValue = SERVO_MIN;

// input/output variables passed by reference, so they are updated automatically
AutoPID servoPID(&currentRPM, &targetRPM, &servoValue, SERVO_MIN, SERVO_MAX, KP, KI, KD);

ServoTimer2 servo;

void setup() {
  Serial.begin(57600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ACTIVATE_PIN, INPUT_PULLUP);
  FreqMeasure.begin();
  servo.attach(SERVO_PIN);
  
  //servoPID.setBangBang(BANGBANG_LIMIT);
  servoPID.setTimeStep(200);
}

void loop() {
  if (FreqMeasure.available()) {
    double count = FreqMeasure.read();
    float frequency = FreqMeasure.countToFrequency(count);
    currentRPM = frequency * DEGREES_PER_PULSE / 360 * 60;
    Serial.println(currentRPM);
    digitalWrite(LED_BUILTIN, servoPID.atSetPoint(BANGBANG_LIMIT));
  }

  if (digitalRead(ACTIVATE_PIN) == LOW) {
    servoPID.run();
    servo.write(servoValue);
    //digitalWrite(LED_BUILTIN, HIGH);
  } else {
    servoPID.stop();
    servo.write(SERVO_MIN);
    //digitalWrite(LED_BUILTIN, LOW);
  }
}
