#include <Servo.h> 
Servo servoObject;
Servo servoObject2;
// ### Bot Status ###
int servoPosition = 90;
int angleX = 90;
int angleY = 90;
bool manualMode = true;
bool modeChanged = false;
int ignore = 100;
extern void changeCardText(String name, String text);

//#######################   Functions Motor

void initMotorPins() {
  ledcAttachPin(in1, in1ALEDChannel);
  ledcAttachPin(in2, in2ALEDChannel);
  ledcAttachPin(in3, in1BLEDChannel);
  ledcAttachPin(in4, in2BLEDChannel);
  ledcSetup(in1ALEDChannel, 20000, 10); // 2 kHz PWM, 10-bit resolution
  ledcSetup(in1BLEDChannel, 20000, 10); // 2 kHz PWM, 10-bit resolution
  ledcSetup(in2ALEDChannel, 20000, 10); // 2 kHz PWM, 10-bit resolution
  ledcSetup(in2BLEDChannel, 20000, 10); // 2 kHz PWM, 10-bit resolution
  ledcWrite(in1ALEDChannel, 0);
  ledcWrite(in2ALEDChannel, 0);
  ledcWrite(in1BLEDChannel, 0);
  ledcWrite(in2BLEDChannel, 0);
  // Servo
  servoObject.attach(servoPinH, 600, 2380);
  servoObject2.attach(servoPinG, 600, 2380);
}

/**
   Function to convert the touch inputs from the Website to a motor speed.
*/
int angleToMotorSpeed (int angle) {
  long motorSpeed = 0;
  motorSpeed = angle * (1023 / 45) - 2048;
  if (motorSpeed < -1023) {
    motorSpeed = -1023;
  }
  if (motorSpeed > 1023) {
    motorSpeed = 1023;
  }
  return motorSpeed;
}

void setMotorSpeed(int leftMotor, int rightMotor) {
  if (configSet.swapMotors) {
    int tempt = leftMotor;
    leftMotor = rightMotor;
    rightMotor = tempt;
  }
  if (configSet.invertLeftMotor) {
    leftMotor = leftMotor *-1;
  }
  if (configSet.invertRightMotor) {
    rightMotor = rightMotor *-1;
  }
  changeCardText("left", "MotorL: " + String(map(leftMotor, 0, 1024, 0, 100) ) + " % ");
  changeCardText("right", "MotorR: " + String(map(rightMotor, 0, 1024, 0, 100)) + " % ");
  if (leftMotor < - ignore) {   //Turn left
    leftMotor = map(leftMotor, 0, -1024, -configSet.deadBand, -1024);
    ledcWrite(in1ALEDChannel, - leftMotor);
    ledcWrite(in2ALEDChannel, 0);
    #ifdef DEBUG
    Serial.println("Left motor: " + String((leftMotor*-1)));
    #endif
  } else if (leftMotor > ignore) {
    leftMotor = map(leftMotor, 0, 1024, configSet.deadBand, 1024);
    ledcWrite(in2ALEDChannel, leftMotor);
    ledcWrite(in1ALEDChannel, 0);
    #ifdef DEBUG
    Serial.println("Left motor: " + String(leftMotor));
    #endif
  } else {
    ledcWrite(in1ALEDChannel, 0);
    ledcWrite(in2ALEDChannel, 0);
    #ifdef DEBUG
    Serial.println("Left motor: STOPPED");
    #endif
    changeCardText("left", "MotorL: 0% ");
    }

  if (rightMotor < - ignore) {   //Turn left
    rightMotor = map(rightMotor, 0, -1024, -configSet.deadBand, -1024);
    ledcWrite(in1BLEDChannel, - rightMotor);
    ledcWrite(in2BLEDChannel, 0);
    #ifdef DEBUG
    Serial.println("Right motor: " + String((rightMotor*-1)));
    #endif
  } else if (rightMotor > ignore) {
    rightMotor = map(rightMotor, 0, 1024, configSet.deadBand, 1024);
    ledcWrite(in2BLEDChannel, rightMotor);
    ledcWrite(in1BLEDChannel, 0);
    #ifdef DEBUG
    Serial.println("Right motor: " + String(rightMotor));
    #endif
  } else {
    ledcWrite(in1BLEDChannel, 0);
    ledcWrite(in2BLEDChannel, 0);
    #ifdef DEBUG
    Serial.println("Right motor: STOPED");
    #endif
    changeCardText("right", "MotorR: 0% ");
  }
}

void setServo(int pos) {
  servoObject.write(pos);
  changeCardText("servoC", "Servo: " + String(pos) + "° ");
}
