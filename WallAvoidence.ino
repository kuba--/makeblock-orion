#ifndef WALL_AVOIDENCE_INO
#define WALL_AVOIDENCE_INO

#include <MeOrion.h>

#define MIN_DISTANCE_CM 15.0
#define SPEED 150
#define DELAY_MS 500
#define LEFT 1
#define RIGHT -1

MeUltrasonicSensor d(PORT_3); // distance
MeDCMotor mL(M1);
MeDCMotor mR(M2);

static void fwd() {
  mL.run(-SPEED);
  mR.run(SPEED);
}

static void turn(uint8_t dir) {
  mL.run(dir * SPEED);
  mR.run(dir * SPEED);
}

static double dist() {
  double cm = d.distanceCm();
  Serial.println(cm);
  delay(100);
  return cm;
}

static void avoid() {
  static uint8_t lastdir = RIGHT;

  uint8_t dir = random(RIGHT, LEFT);
  if (!dir)
    dir = -lastdir;

  turn(dir);
  delay(DELAY_MS);

  lastdir = dir;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (dist() < MIN_DISTANCE_CM) {
    avoid();
  }

  fwd();
}

#endif
