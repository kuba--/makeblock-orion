#ifndef WALL_AVOIDANCE_INO
#define WALL_AVOIDANCE_INO

#include <MeOrion.h>

#define MIN_DISTANCE_CM 15.0
#define SPEED 150

MeUltrasonicSensor sonar(PORT_3);
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

static void avoid() {
  uint8_t dir = random(0, 2);
  if (!dir)
    dir = -1;

  turn(dir);
}

void setup() {
  //Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop() {
  double d = sonar.distanceCm();
  //Serial.println(d);
  if (d < MIN_DISTANCE_CM)
    avoid();
  else
    fwd();

  delay(200);
}
#endif
