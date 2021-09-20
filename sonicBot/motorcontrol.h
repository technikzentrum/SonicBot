

// ### Bot Status ###
int angleX = 90;
int angleY = 90;
bool manualMode = true;

//#######################   Functions Motor

void initMotorPins() {
  #ifdef ESP32
  ledcAttachPin(enA, enALEDChannel);
  ledcAttachPin(enB, enBLEDChannel);
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(enALEDChannel, 20000, 10); // 2 kHz PWM, 10-bit resolution
  ledcSetup(enBLEDChannel, 20000, 10); // 2 kHz PWM, 10-bit resolution
  #else
  analogWriteRange(1023);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  #endif
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
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
  if (leftMotor < - configSet.deadBand) {   //Turn left
    #ifdef ESP32
    ledcWrite(enALEDChannel, - leftMotor);
    #else
    analogWrite(enA, - leftMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    #ifdef DEBUG
    Serial.println("Left motor: " + String((leftMotor*-1)));
    #endif
  } else if (leftMotor > configSet.deadBand) {
    #ifdef ESP32
    ledcWrite(enALEDChannel, leftMotor);
    #else
    analogWrite(enA, leftMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    #ifdef DEBUG
    Serial.println("Left motor: " + String(leftMotor));
    #endif
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    #ifdef DEBUG
    Serial.println("Left motor: STOPPED");
    #endif
  }

  if (rightMotor < - configSet.deadBand) {   //Turn left
    #ifdef ESP32
    ledcWrite(enBLEDChannel, - rightMotor);
    #else
    analogWrite(enB, - rightMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    #ifdef DEBUG
    Serial.println("Right motor: " + String((rightMotor*-1)));
    #endif
  } else if (rightMotor > configSet.deadBand) {
    #ifdef ESP32
    ledcWrite(enBLEDChannel, rightMotor);
    #else
    analogWrite(enB, rightMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    #ifdef DEBUG
    Serial.println("Right motor: " + String(rightMotor));
    #endif
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    #ifdef DEBUG
    Serial.println("Right motor: STOPED");
    #endif
  }
}
