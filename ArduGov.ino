/*
 * ArduGov - A standalone Arduino based engine governor.
 * 
 * Pins assignments:
 * 7 - Governor on/off switch.
 * 8 - Tach pulse input
 * 9 - Servo output
 */
  
// Target RPM value
#define RPM_SETPOINT 3600

// Degrees of rotation per tach pulse
#define DEGREES_PER_PULSE 20

// Activation pin
#define ACTIVATE_PIN 7

// Servo pin and limits in uS
#define SERVO_PIN 9
#define SERVO_MIN 900
#define SERVO_MAX 2100

// PID settings
#define KP .12
#define KI .0003
#define KD 0

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
  
  servoPID.setBangBang(200); // BangBang <> 200rpm from setpoint
  servoPID.setTimeStep(50); // Calculate every 50ms
}

void loop() {
  if (FreqMeasure.available()) {
    float frequency = FreqMeasure.countToFrequency(FreqMeasure.read());
    currentRPM = frequency * DEGREES_PER_PULSE / 360 * 60;
    Serial.println(frequency);
    digitalWrite(LED_BUILTIN, servoPID.atSetPoint(25));
  }

  if (digitalRead(ACTIVATE_PIN) == LOW) {
    servoPID.run();
    servo.write(servoValue);
  } else {
    servoPID.stop();
    servo.write(SERVO_MIN);
  }
}
