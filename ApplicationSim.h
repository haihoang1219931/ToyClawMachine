#ifndef APPLICATIONSIM_H
#define APPLICATIONSIM_H

#include <QObject>
#include <QThread>
#include <QtMultimedia/QMediaPlayer>
#include "ApplicationController.h"
class MainProcess;
class ApplicationSim : public ApplicationController
{
public:
    ApplicationSim(MainProcess* mainProcess);
    ~ApplicationSim();
    int printf(const char *fmt, ...) override;
    void msleep(int millis) override;
    void enableAxis(MACHINE_AXIS axis, bool enable)override;
    bool isNewCoinInserted() override;
    bool isButtonPressed() override;
    bool isJoystickPressed(JOYSTICK_DIR dir) override;
    void setAxisDir(MACHINE_AXIS axis, int dir) override;
    bool isAxisAtMin(MACHINE_AXIS axis) override;
    bool isAxisAtMax(MACHINE_AXIS axis) override;
    void moveAxis(MACHINE_AXIS axis) override;
    void activeClaw(bool active) override;
    bool isClawAtMin() override;
    bool isClawAtMax() override;
    void activeJoystickLED(bool active) override;
    void activeButtonLED(bool active) override;
    void updateCoin(int coin) override;
    void showStateMachine(MACHINE_STATE newState) override;
    void playSound(SOUND_ID soundID, bool repeat = false) override;
    void pauseSound() override;
    void continueSound(SOUND_ID soundID, int position, bool repeat = true) override;
    bool soundStopped(SOUND_ID soundID) override;
    int getSoundPosition(SOUND_ID soundID) override;
public:
    bool m_coinSignalHigh;
    long m_coinSignalHighTime;
    long m_coinSignalLowTime;
    bool m_coinSignalHandling;
    bool m_buttonSignalHigh;
    long m_buttonSignalHighTime;
    long m_buttonSignalLowTime;
    bool m_buttonSignalHandling;
    bool m_jsDirPressed[JOYSTICK_DIR::JS_TOTAL_DIR];
private:
    int m_axisValue[MACHINE_AXIS::AXIS_MAX];
    int m_axisValueMin[MACHINE_AXIS::AXIS_MAX];
    int m_axisValueMax[MACHINE_AXIS::AXIS_MAX];
    int m_clawValue;
    int m_clawValueMin;
    int m_clawValueMax;
    MainProcess* m_mainProcess;
    QMediaPlayer* m_player;
    QString m_soundList[10];
};

#endif // APPLICATIONSIM_H
