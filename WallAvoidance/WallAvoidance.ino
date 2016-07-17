#ifndef WALL_AVOIDANCE_INO
#define WALL_AVOIDANCE_INO

#include <MeOrion.h>

#define DEBUG 1

#define MAX_SOUND_STRENGTH 512
#define MIN_DISTANCE_CM 15.0
#define SPEED 160
#define TURN_DELAY_MS 1000
#define DEFAULT_DELAY_MS 200

MeSoundSensor SoundSensor(PORT_8);
MeRGBLed Led(PORT_3, SLOT_1, 15);
MeUltrasonicSensor UltrasonicSensor(PORT_4);
MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);
const int16_t Degrees[2] = { -90, 90 };
const int16_t Noises[2] = { 150, 256 };

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  randomSeed(analogRead(0));
}

void loop() {

  int16_t strength = soundStrength();
  setLed(strength);

  double cm = distanceCm();
  if (cm > 0.0 && cm < MIN_DISTANCE_CM)
    turn(Degrees[random(0, 2)]);

  moveForward();

  delay(DEFAULT_DELAY_MS);
}

static int16_t soundStrength() {
  int16_t strength = SoundSensor.strength();
#ifdef DEBUG
  Serial.print("sound.strength: ");
  Serial.println(strength);
#endif
  return strength;
}

static void setLed(int16_t val) {
  double f = 256.0;

  if (val < Noises[0]) {
    f /= Noises[0];

    Led.setColor(0, 0, val * ceil(f));
    Led.show();

    return;
  }

  if (val < Noises[1]) {
    val -= Noises[0];
    f /= (Noises[1] - Noises[0]);

    Led.setColor(0, val * ceil(f), 0);
    Led.show();

    return;
  }

  val -= Noises[1];
  if (val < 1) val = 1;
  if (val > 255) val = 255;

  Led.setColor(val, 0, 0);
  Led.show();
}

static double distanceCm() {
  double cm = UltrasonicSensor.distanceCm();
#ifdef DEBUG
  Serial.print("distance cm: ");
  Serial.println(cm);
#endif
  return cm;
}

// degrees: -180 to 180. Positive number for turning right, negative number for turning left.
static void turn(int16_t degrees) {
  if (degrees == 0) return;

  byte dir = (degrees < 0) ? 1 : -1;
  double ms = TURN_DELAY_MS * (abs(degrees) / 180.0);

  MotorL.run(dir * SPEED);
  MotorR.run(dir * SPEED);

  delay(round(ms));
}

static void moveForward() {
  MotorL.run(-SPEED);
  MotorR.run(SPEED);
}

#endif
