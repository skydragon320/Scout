#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// robot configuration
const int NUM_LEGS        = 6;
const int JOINTS_PER_LEG  = 3;
const int SERVOS_TOTAL    = NUM_LEGS * JOINTS_PER_LEG;

// joint indices
const int COXA  = 0;
const int FEMUR = 1;
const int TIBIA = 2;

// home angles
const float COXA_HOME  = 90.0;
const float FEMUR_HOME = 45.0;
const float TIBIA_HOME = 30.0;

// link lengths in mm
const float COXA_LEN  = 50.0;
const float FEMUR_LEN = 90.0;
const float TIBIA_LEN = 165.0;

// servo pulse limits
const int SERVO_MIN = 150;
const int SERVO_MAX = 600;

// outside class so visible everywhere
struct ServoAddress {
  Adafruit_PWMServoDriver* board;
  int channel;
};

class Leg {
  public:
    int id;
    int coxaChannel;
    int femurChannel;
    int tibiaChannel;
    float mountAngle;

    int coxaTrim  = 0;
    int femurTrim = 0;
    int tibiaTrim = 0;

    Leg(int id, int coxaCh, int femurCh, int tibiaCh, float mountAngle)
      : id(id), coxaChannel(coxaCh), femurChannel(femurCh),
        tibiaChannel(tibiaCh), mountAngle(mountAngle) {}

    bool moveTo(float x, float y, float z, Adafruit_PWMServoDriver &pwm) {
      float coxaAngle = atan2(y, x) * RAD_TO_DEG;
      float horizontalDist = sqrt(x * x + y * y) - COXA_LEN;
      float L = sqrt(horizontalDist * horizontalDist + z * z);

      if (L > FEMUR_LEN + TIBIA_LEN) {
        Serial.print("Leg "); Serial.print(id);
        Serial.println(": target out of reach");
        return false;
      }

      float cosTimbia = constrain(
        (L * L - FEMUR_LEN * FEMUR_LEN - TIBIA_LEN * TIBIA_LEN)
        / (2.0 * FEMUR_LEN * TIBIA_LEN), -1.0, 1.0);
      float tibiaAngle = acos(cosTimbia) * RAD_TO_DEG;

      float alpha = atan2(-z, horizontalDist) * RAD_TO_DEG;
      float beta  = acos(constrain(
        (L * L + FEMUR_LEN * FEMUR_LEN - TIBIA_LEN * TIBIA_LEN)
        / (2.0 * L * FEMUR_LEN), -1.0, 1.0)) * RAD_TO_DEG;
      float femurAngle = alpha + beta;

      Serial.print("Leg "); Serial.print(id);
      Serial.print(" | Coxa: ");  Serial.print(coxaAngle);
      Serial.print(" Femur: "); Serial.print(femurAngle);
      Serial.print(" Tibia: "); Serial.println(tibiaAngle);

      writeServo(pwm, coxaChannel,  coxaAngle  + COXA_HOME,  coxaTrim);
      writeServo(pwm, femurChannel, femurAngle + FEMUR_HOME, femurTrim);
      writeServo(pwm, tibiaChannel, tibiaAngle + TIBIA_HOME, tibiaTrim);

      return true;
    }

    void setHome(Adafruit_PWMServoDriver &pwm) {
      moveTo(COXA_LEN + 60, 0, -80, pwm);
    }

  private:
    void writeServo(Adafruit_PWMServoDriver &pwm, int channel,
                    float angle, int trim) {
      int pulse = map((int)(angle + trim), 0, 180, SERVO_MIN, SERVO_MAX);
      pulse = constrain(pulse, SERVO_MIN, SERVO_MAX);
      pwm.setPWM(channel, 0, pulse);
    }
};