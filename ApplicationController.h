#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include <stdio.h>
#include <cstring>
#include <stdarg.h>
enum MACHINE_STATE {
    WAIT_INPUT,
    WAIT_READY,
    MOVE_GANTRY,
    DROP_CLAW,
    CLOSE_CLAW,
    RETURN_CLAW,
    MOVE_HOME,
    OPEN_CLAW
};

enum MACHINE_AXIS {
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
    AXIS_MAX,
};

enum JOYSTICK_DIR {
    JS_UP,
    JS_DOWN,
    JS_LEFT,
    JS_RIGHT,
    JS_TOTAL_DIR
};

enum SOUND_ID {
    SOUND_BGM,
    SOUND_NEEDCOIN,
    SOUND_NEWCOIN,
    SOUND_CLAW_DROP,
    SOUND_CLAW_HIT_TOP,
    SOUND_CLAW_UP,
    SOUND_CLAW_WIN,
};


class ApplicationController
{
public:
    ApplicationController();
    ~ApplicationController();

    void loopFunction();
    void resetGame();
    void acceptCoin();
    void removeCoin();
    virtual int printf(const char *fmt, ...) = 0;
    virtual void msleep(int millis) = 0;
protected:
    void waitInput();
    void waitReady();
    void moveGantry();
    void dropClaw();
    void closeClaw();
    void returnClaw();
    void moveHome();
    void openClaw();
    void setMachineState(MACHINE_STATE newState);
    virtual void enableAxis(MACHINE_AXIS axis, bool enable) = 0;
    virtual bool isNewCoinInserted() = 0;
    virtual bool isButtonPressed() = 0;
    virtual bool isJoystickPressed(JOYSTICK_DIR dir) = 0;
    virtual void setAxisDir(MACHINE_AXIS axis, int dir) = 0;
    virtual bool isAxisAtMin(MACHINE_AXIS axis) = 0;
    virtual bool isAxisAtMax(MACHINE_AXIS axis) = 0;
    virtual void moveAxis(MACHINE_AXIS axis) = 0;
    virtual void activeClaw(bool active) = 0;
    virtual bool isClawAtMin() = 0;
    virtual bool isClawAtMax() = 0;
    virtual void activeJoystickLED(bool active) = 0;
    virtual void activeButtonLED(bool active) = 0;
    virtual void updateCoin(int coin) = 0;
    virtual void showStateMachine(MACHINE_STATE newState) = 0;
    virtual void playSound(SOUND_ID soundID, bool repeat = false) = 0;
    virtual void pauseSound() = 0;
    virtual void continueSound(SOUND_ID soundID, int position, bool repeat = true) = 0;
    virtual bool soundStopped(SOUND_ID soundID) = 0;
    virtual int getSoundPosition(SOUND_ID soundID) = 0;

public:
    MACHINE_STATE m_machineState;
    
    int m_coin;

    bool m_axisEnable[MACHINE_AXIS::AXIS_MAX];
    int m_axisDir[MACHINE_AXIS::AXIS_MAX];

    int m_clawEnable;
    int m_clawDir;

    bool m_joystickLEDEnable;
    bool m_buttonLEDEnable;

    int m_bgmStopPosition;
};

#endif // APPLICATIONCONTROLLER_H
