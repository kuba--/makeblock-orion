#ifndef DrinkCar_ino
#define DrinkCar_ino

#include <MeOrion.h>

#define DEFAULT_INTERVAL_MS 100

#define MIN_DISTANCE_CM 8

#define LIFT_DOWN_MS 7800
#define LIFT_DOWN_SPEED (-50)

#define LIFT_UP_MS 3500
#define LIFT_UP_SPEED 100

#define MAX_SPEED 100
#define MIN_SPEED 10

#define LEFT S1_IN_S2_OUT
#define RIGHT S1_OUT_S2_IN

#ifdef DEBUG
#define DBG(...) Serial.println(__VA_ARGS__)
#else
#define DBG(...)
#endif

#define Run(speed)          \
    do {                    \
        MotorL.run(-speed); \
        MotorR.run(speed);  \
    } while (0)

#define Turn(dir, outer, inner)     \
    do {                            \
        switch (dir) {              \
            case LEFT:              \
                MotorL.run(-inner); \
                MotorR.run(outer);  \
                break;              \
            case RIGHT:             \
                MotorL.run(-outer); \
                MotorR.run(inner);  \
                break;              \
            default:                \
                Run(outer);         \
        }                           \
    } while (0)

#define Stop(...)      \
    do {               \
        MotorL.stop(); \
        MotorR.stop(); \
    } while (0)

#define Pour(...)                  \
    do {                           \
        Lift.run(LIFT_DOWN_SPEED); \
        delay(LIFT_DOWN_MS);       \
        Lift.run(LIFT_UP_SPEED);   \
        delay(LIFT_UP_MS);         \
        Lift.stop();               \
    } while (0)

MeLineFollower LineFollower(PORT_3);
MeUltrasonicSensor UltrasonicSensor(PORT_6);
MeDCMotor MotorL(M1), MotorR(M2);
MeDCMotor Lift(PORT_1);

enum { STOP, RUN, TURN } State = RUN;
uint8_t Direction = RIGHT;
uint8_t SpeedOuter = MAX_SPEED, SpeedInner = MAX_SPEED;

typedef void (*Task)(void);

void taskFollowLine() {
    if (State == STOP) return;

    uint8_t dir = LineFollower.readSensors();
    if (dir == S1_OUT_S2_OUT) {
        State = TURN;
        SpeedInner /= 2;
        if (SpeedInner < MIN_SPEED) SpeedInner = MIN_SPEED;
        Turn(Direction, SpeedOuter, SpeedInner);
    } else {
        Direction = dir;
        State = RUN;
        SpeedInner = SpeedOuter;
        Run(SpeedOuter);
    }
}

void taskPourDrink() {
    static unsigned long lastcall = 0;
    unsigned long now = millis();
    if (now < lastcall + DEFAULT_INTERVAL_MS) return;

    double d = UltrasonicSensor.distanceCm();
    DBG("distanceCm:", d);

    if (d > 0.0 && d < MIN_DISTANCE_CM) {
        if (State != STOP) {
            State = STOP;
            Stop();
            Pour();
        }
    } else if (State == STOP) {
        State = RUN;
        Run(SpeedOuter);
    }

    lastcall = now;
}

const Task Tasks[] = {taskFollowLine, taskPourDrink};
const uint8_t NTasks = sizeof(Tasks) / sizeof(Task);

void setup() {
#ifdef DEBUG
    Serial.begin(9600);
#endif
}

void loop() {
    static uint8_t i = 0;

    Tasks[i]();
    i = (i + 1) % NTasks;
}

#endif
