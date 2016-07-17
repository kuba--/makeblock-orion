#ifndef StopAndGo_ino
#define StopAndGo_ino

#include <MeOrion.h>

#define DEBUG 0

#define MAX_SOUND_STRENGTH 550
#define MIN_DISTANCE_CM 20.0
#define MAX_SOUND_INTERVAL_MS 1000
#define DEFAULT_INTERVAL_MS 200
#define TURN90_DELAY_MS 500
#define SPEED 160

typedef void (* Event)(void);

void onNoise();
void onWall();
void onState();

enum {
  STOP,
  TURN,
  GO
} State;

enum {
  LEFT,
  RIGHT
} Direction;


const int16_t Noises[2] = { 256, 512 };
const Event Events[] = { onNoise, onWall, onState };
const uint8_t NEvents =  sizeof(Events) / sizeof(Event);

MeSoundSensor SoundSensor(PORT_8);
MeRGBLed Led(PORT_3, SLOT_1, 15);
MeUltrasonicSensor UltrasonicSensor(PORT_4);
MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  randomSeed(analogRead(0));

  State = STOP;
  Direction = LEFT;
}

void loop() {
  static uint8_t i = 0;

  Events[i]();
  i = (i + 1) % NEvents;
}

static int16_t soundStrength() {
  int16_t strength = SoundSensor.strength();
#ifdef DEBUG
  Serial.print("sound.strength: ");
  Serial.println(strength);
#endif
  return strength;
}

static void ledShow(int16_t val) {
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
  double d = UltrasonicSensor.distanceCm();
#ifdef DEBUG
  Serial.print("distance cm: ");
  Serial.println(d);
#endif
  return d;
}

static void moveForward() {
  MotorL.run(-SPEED);
  MotorR.run(SPEED);
}

static void stopMotors() {
  MotorL.stop();
  MotorR.stop();
}

static void turn90() {
  int16_t l = -SPEED, r = SPEED;
  if (Direction == LEFT)
    l = -l;
  else
    r = -r;

  MotorL.run(l);
  MotorR.run(r);
  delay(TURN90_DELAY_MS);
}

void onNoise() {
  static unsigned long lastcall = 0;
  static unsigned long lastnoise = 0;
  unsigned long now = millis();

  if (now < lastcall + DEFAULT_INTERVAL_MS)
    return;

  int16_t strength = soundStrength();
  ledShow(strength);

  if (strength > MAX_SOUND_STRENGTH && now >= lastnoise + MAX_SOUND_INTERVAL_MS) {
    State = (State == STOP) ? GO : STOP;
    lastnoise = now;
  }

  lastcall = now;
}

void onWall() {
  static unsigned long lastcall = 0;
  unsigned long now = millis();

  if (State == STOP || now < lastcall + DEFAULT_INTERVAL_MS)
    return;

  double d = distanceCm();
  if (d > 0.0 && d < MIN_DISTANCE_CM) {
    if (State != TURN)
      Direction = (0 == random(0, 2)) ? LEFT : RIGHT;
    State = TURN;    
  } else {  
    State = GO;
  }

  lastcall = now;
}

void onState() {
  switch (State) {
    case STOP:
      stopMotors();
      break;

    case TURN:
      turn90();
      break;

    case GO:
      moveForward();
      break;
  }
}

#endif
