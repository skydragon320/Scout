#include "leg.h"
#include "controller.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "esp_task_wdt.h"

Adafruit_PWMServoDriver pwmRight = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwmLeft  = Adafruit_PWMServoDriver(0x40);

int legChannels[NUM_LEGS][JOINTS_PER_LEG] = {
  {4,  5,  6},   // leg 0 - right
  {8,  9,  10},  // leg 1 - right
  {12, 13, 14},  // leg 2 - right
  {0,  1,  2},   // leg 3 - left
  {4,  5,  6},   // leg 4 - left
  {8,  9,  10},  // leg 5 - left
};

bool reversed[NUM_LEGS][JOINTS_PER_LEG] = {
  {false, true, true},
  {false, true, true},
  {false, true, true},
  {false, true, true},
  {false, true, true},
  {false, true, true},
};

void writeServo(int leg, int joint, float angle) {
  if (reversed[leg][joint]) angle = 180.0 - angle;
  int pulse = map((int)angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pulse = constrain(pulse, SERVO_MIN, SERVO_MAX);
  if (leg < 3) {
    pwmRight.setPWM(legChannels[leg][joint], 0, pulse);
  } else {
    pwmLeft.setPWM(legChannels[leg][joint], 0, pulse);
  }
}

void safeDelay(int ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    esp_task_wdt_reset();
    cleanupWebSocket(); 
    delay(10);
  }
}

// --- interpolation ---

void interpolateServo(int leg, int joint, float startAngle,
                      float endAngle, int durationMs) {
  unsigned long startTime = millis();
  while (true) {
    esp_task_wdt_reset();
    float t = constrain((float)(millis() - startTime) / durationMs, 0.0, 1.0);
    float tSmooth = (1.0 - cos(t * PI)) / 2.0;
    writeServo(leg, joint, startAngle + (endAngle - startAngle) * tSmooth);
    if (t >= 1.0) break;
  }  
}    

void interpolateAllLegs(int joint, float startAngle,
                        float endAngle, int durationMs) {
  unsigned long startTime = millis();
  while (true) {
    esp_task_wdt_reset();
    float t = constrain((float)(millis() - startTime) / durationMs, 0.0, 1.0);
    float tSmooth = (1.0 - cos(t * PI)) / 2.0;
    float angle = startAngle + (endAngle - startAngle) * tSmooth;
    for (int leg = 0; leg < NUM_LEGS; leg++) writeServo(leg, joint, angle);
    if (t >= 1.0) break;
  }  // ← closes while  (THIS WAS MISSING)
}    // ← closes function (THIS WAS MISSING)

void interpolateTwoJoints(int joint1, float start1, float end1,
                           int joint2, float start2, float end2,
                           int durationMs) {
  unsigned long startTime = millis();
  while (true) {
    esp_task_wdt_reset();
    float t = constrain((float)(millis() - startTime) / durationMs, 0.0, 1.0);
    float tSmooth = (1.0 - cos(t * PI)) / 2.0;
    float a1 = start1 + (end1 - start1) * tSmooth;
    float a2 = start2 + (end2 - start2) * tSmooth;
    for (int leg = 0; leg < NUM_LEGS; leg++) {
      writeServo(leg, joint1, a1);
      writeServo(leg, joint2, a2);
    }
    if (t >= 1.0) break;
  }
}

// --- motion ---

void goHome() {
  for (int leg = 0; leg < NUM_LEGS; leg++) {
    writeServo(leg, COXA,  90);
    writeServo(leg, FEMUR, 90);
    writeServo(leg, TIBIA, 0);
  }
  Serial.println("Homed");
}

void standUp() {
  goHome();
  safeDelay(1000);
  interpolateTwoJoints(FEMUR, 90, 90, TIBIA, 0, 10, 1000);
  safeDelay(400);
  interpolateTwoJoints(FEMUR, 90, 60, TIBIA, 10, 30, 1200);
  safeDelay(500);
  Serial.println("Scout is standing");
}

void wave() {
  Serial.println("Waving...");
  interpolateServo(0, FEMUR, 60, 30, 600);
  interpolateServo(0, TIBIA, 30, 60, 600);
  safeDelay(300);
  interpolateServo(0, COXA, 90, 130, 400);
  safeDelay(200);
  interpolateServo(0, COXA, 130, 50, 400);
  safeDelay(200);
  interpolateServo(0, COXA, 50, 130, 400);
  safeDelay(200);
  interpolateServo(0, COXA, 130, 90, 400);
  safeDelay(200);
  interpolateServo(0, FEMUR, 30, 60, 600);
  interpolateServo(0, TIBIA, 60, 30, 600);
  safeDelay(300);
  Serial.println("Wave complete");
}

void stomp() {
  Serial.println("Stomping...");
  for (int leg = 0; leg < NUM_LEGS; leg++) {
    Serial.print("Stomping leg "); Serial.println(leg);
    interpolateServo(leg, FEMUR, 60, 30, 400);
    interpolateServo(leg, TIBIA, 30, 60, 400);
    safeDelay(200);
    interpolateServo(leg, FEMUR, 30, 60, 300);
    interpolateServo(leg, TIBIA, 60, 30, 300);
    safeDelay(100);
  }
  Serial.println("Stomp complete");
}

void demo() {
  Serial.println("=== Scout Demo ===");
  goHome();
  safeDelay(1000);
  standUp();
  safeDelay(2000);
  stomp();
  safeDelay(1000);
  wave();
  Serial.println("=== Demo Complete ===");
}

void pseudoWalk(int steps) {
  Serial.println("Pseudo walking...");

  // make sure support group is planted first
  // legs 1, 3, 5 hold standing position
  writeServo(1, FEMUR, 60); writeServo(1, TIBIA, 30);
  writeServo(3, FEMUR, 60); writeServo(3, TIBIA, 30);
  writeServo(5, FEMUR, 60); writeServo(5, TIBIA, 30);
  safeDelay(500);

  for (int s = 0; s < steps; s++) {
    Serial.print("Step "); Serial.println(s + 1);

    // phase 1 - lift swing group (0, 2, 4)
    for (int leg : {0, 2, 4}) {
      interpolateServo(leg, FEMUR, 60, 30, 400);
      interpolateServo(leg, TIBIA, 30, 60, 400);
    }
    safeDelay(200);

    // phase 2 - swing forward (coxa forward)
    for (int leg : {0, 2, 4}) {
      interpolateServo(leg, COXA, 90, 70, 400);
    }
    safeDelay(200);

    // phase 3 - plant swing group
    for (int leg : {0, 2, 4}) {
      interpolateServo(leg, FEMUR, 30, 60, 400);
      interpolateServo(leg, TIBIA, 60, 30, 400);
    }
    safeDelay(200);

    // phase 4 - push back (all coxas sweep back)
    // this is what actually moves the body forward
    // phase 4 - lift support group slightly and push
// unload support legs so they dont resist body movement
    for (int leg : {1, 3, 5}) {
      interpolateServo(leg, FEMUR, 60, 45, 300); // lift slightly
    }
    safeDelay(200);

    // now push with all coxas
    for (int leg = 0; leg < NUM_LEGS; leg++) {
      interpolateServo(leg, COXA, 70, 110, 500);
    }
    safeDelay(200);

    // plant support group back down
    for (int leg : {1, 3, 5}) {
      interpolateServo(leg, FEMUR, 45, 60, 300);
    }
    safeDelay(150);
    

    // phase 5 - reset swing group coxa to center
    // support group stays pushed back ready for next step
    for (int leg : {0, 2, 4}) {
      interpolateServo(leg, COXA, 110, 90, 300);
    }
    safeDelay(100);
  }

  // return all coxas to center after walking
  for (int leg = 0; leg < NUM_LEGS; leg++) {
    interpolateServo(leg, COXA, 110, 90, 400);
  }

  Serial.println("Walk complete");
}

void printCommands() {
  Serial.println("=== Scout Commands ===");
  Serial.println("stand  → standup sequence");
  Serial.println("stomp  → stomp each leg clockwise");
  Serial.println("wave   → wave leg 0");
  Serial.println("home   → all servos to start position");
  Serial.println("demo   → full demo sequence");
  Serial.println("walk   → pseudo walk forward 3 steps");
  Serial.println("?      → show commands");
}

String buffer = "";

void handleSerial() {
  while (Serial.available() > 0) {
    int c = Serial.read();
    if (c == '\n') {
      buffer.trim();
      if      (buffer == "stand") standUp();
      else if (buffer == "wave")  wave();
      else if (buffer == "home")  goHome();
      else if (buffer == "stomp") stomp();
      else if (buffer == "demo")  demo();
      else if (buffer == "walk") pseudoWalk(3);
      else if (buffer == "?")     printCommands();
      else {
        Serial.print("Unknown: ");
        Serial.println(buffer);
      }
      buffer = "";
    } else if (c != '\r') {
      buffer += (char)c;
    }
  }
}

void setup() {
  Serial.begin(115200);

  pwmRight.begin();
  //pwmRight.setOscillatorFrequency(27000000);
  pwmRight.setPWMFreq(60);
  pwmRight.wakeup();

  pwmLeft.begin();
  pwmLeft.setPWMFreq(60);
  pwmLeft.wakeup();

  delay(500);

  controllerBegin();

  Serial.println("Scout ready");
  printCommands();
}

void loop() {
  controllerLoop();
  handleSerial();
}