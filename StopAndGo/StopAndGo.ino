#ifndef StopAndGo_ino
#define StopAndGo_ino

#include <MeOrion.h>

#define DEFAULT_INTERVAL_MS 200

#define MAX_SOUND_STRENGTH 550
#define MIN_DISTANCE_CM 20.0

#define MAX_SOUND_INTERVAL_MS 1000
#define TURN90_DELAY_MS 500

#define SPEED 160

#ifdef DEBUG
#define DBG(...) Serial.println(__VA_ARGS__)
#else
#define DBG(...)
#endif

#define Stop(...)      \
    do {               \
        MotorL.stop(); \
        MotorR.stop(); \
    } while (0)

#define Go(...)             \
    do {                    \
        MotorL.run(-SPEED); \
        MotorR.run(SPEED);  \
    } while (0)

#define Turn90(...)                    \
    do {                               \
        int16_t l = -SPEED, r = SPEED; \
        switch (Direction) {           \
            case LEFT:                 \
                l = -l;                \
                break;                 \
            case RIGHT:                \
                r = -r;                \
                break;                 \
        }                              \
        MotorL.run(l);                 \
        MotorR.run(r);                 \
        delay(TURN90_DELAY_MS);        \
    } while (0)

MeSoundSensor SoundSensor(PORT_8);
MeRGBLed Led(PORT_3, SLOT_1, 15);
MeUltrasonicSensor UltrasonicSensor(PORT_4);
MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);

enum { STOP, GO, TURN90 } State = STOP;
enum { LEFT, RIGHT } Direction = LEFT;

typedef void (*Task)(void);

static void showLed(int16_t val) {
    const int16_t noises[2] = {256, 512};
    double f = 256.0;

    if (val < noises[0]) {
        f /= noises[0];

        Led.setColor(0, 0, val * ceil(f));
        Led.show();

        return;
    }

    if (val < noises[1]) {
        val -= noises[0];
        f /= (noises[1] - noises[0]);

        Led.setColor(0, val * ceil(f), 0);
        Led.show();

        return;
    }

    val -= noises[1];
    if (val < 1) val = 1;
    if (val > 255) val = 255;

    Led.setColor(val, 0, 0);
    Led.show();
}

void taskCheckNoise() {
    static unsigned long lastcall = 0;
    static unsigned long lastnoise = 0;
    unsigned long now = millis();

    if (now < lastcall + DEFAULT_INTERVAL_MS) return;

    int16_t strength = SoundSensor.strength();
    DBG("SoundSensor.strength:", strength);

    showLed(strength);

    if (strength > MAX_SOUND_STRENGTH &&
        now >= lastnoise + MAX_SOUND_INTERVAL_MS) {
        State = (State == STOP) ? GO : STOP;
        lastnoise = now;
    }

    lastcall = now;
}

void taskAvoidWall() {
    static unsigned long lastcall = 0;
    unsigned long now = millis();

    if (State == STOP || now < lastcall + DEFAULT_INTERVAL_MS) return;

    double d = UltrasonicSensor.distanceCm();
    DBG("UltrasonicSensor.distanceCm:", d);

    if (d > 0.0 && d < MIN_DISTANCE_CM) {
        if (State != TURN90) Direction = (0 == random(0, 2)) ? LEFT : RIGHT;
        State = TURN90;
    } else {
        State = GO;
    }

    lastcall = now;
}

void taskChangeState() {
    switch (State) {
        case STOP:
            Stop();
            break;

        case TURN90:
            Turn90();
            break;

        case GO:
            Go();
            break;
    }
}

const Task Tasks[] = {taskCheckNoise, taskAvoidWall, taskChangeState};
const uint8_t NTasks = sizeof(Tasks) / sizeof(Task);

void setup() {
#ifdef DEBUG
    Serial.begin(9600);
#endif

    randomSeed(analogRead(0));
}

void loop() {
    static uint8_t i = 0;

    Tasks[i]();
    i = (i + 1) % NTasks;
}

#endif
