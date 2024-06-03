#ifndef APPLICATIONSTM32_H
#define APPLICATIONSTM32_H

#include "ApplicationController.h"
#include "stm32f1xx_hal.h"
#include "TM1637.h"
class UART_HandleTypeDef;
class TM1637;
class ApplicationSTM32 : public ApplicationController
{
public:
    ApplicationSTM32(UART_HandleTypeDef *huart);
    ~ApplicationSTM32();
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
    QString m_soundList[10];
};

#endif // APPLICATIONSTM32_H
