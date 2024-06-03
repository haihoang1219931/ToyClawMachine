#include "ApplicationSTM32.h"

ApplicationSTM32::ApplicationSTM32(UART_HandleTypeDef *huart)
{
    m_huart = huart;
    m_coin = 9999;
    setMachineState(MACHINE_STATE::WAIT_INPUT);
    resetGame();
    m_tm1637 = new TM1637(GPIOB, GPIO_PIN_9, GPIOB, GPIO_PIN_8);
    m_tm1637->TM1637_SetBrightness(7);
    m_tm1637->TM1637_DisplayDecimal(0,false);
}

int ApplicationSTM32::printf(const char *fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    int rc = vsprintf(buffer, fmt, args);
    va_end(args);
    HAL_UART_Transmit(m_huart, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
    return rc;
}

void ApplicationController::delayMicrosec(int micro) {
    for(int i=0; i< micro; i++) {

    }
}

void ApplicationController::waitInput() {
    if(m_axisXEnable) {
        m_axisXEnable = false;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1,GPIO_PIN_SET);
    }

    if(m_axisYEnable) {
        m_axisYEnable = false;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_SET);
    }

    if(m_axisZEnable) {
        m_axisZEnable = false;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,GPIO_PIN_SET);
    }
    // get coin input
    uint32_t millis = HAL_GetTick();
    bool coinSignal = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET);

    if(!m_coinSignal && coinSignal) {
        m_coinSignal = true;
        m_coinTime = millis;

    }else if(m_coinSignal && !coinSignal) {
        m_coinSignal = false;
        this->printf("coinSignal[%s] m_coinTime[%d] millis[%d]\r\n",
                coinSignal?"YES":"   ",
                m_coinTime,millis);
        if(millis - m_coinTime > 75) {
            m_coin ++;
            this->printf("m_coin [%d]\r\n",m_coin);
            m_tm1637->TM1637_DisplayDecimal(m_coin,false);
        }
    }
    bool clawPressed = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET;
    if(!clawPressed && m_clawPressed) {
        m_clawPressed = false;
    } else if(m_coin >= 1 && clawPressed && !m_clawPressed) {
        m_clawPressed = true;
        m_coin --;
        m_tm1637->TM1637_DisplayDecimal(m_coin,false);
        m_clawPressTime = millis;
        this->printf("Start game m_coin[%d]\r\n",m_coin);
#ifdef QT_SIMULATOR
        Q_EMIT coinChanged(m_coin);
#endif
        setMachineState(MACHINE_STATE::MOVE_GANTRY);
    }
    HAL_Delay(1);
}

void ApplicationController::moveGantry() {
    if(!m_axisXEnable) {
        m_axisXEnable = true;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1,GPIO_PIN_RESET);
    }

    if(!m_axisYEnable) {
        m_axisYEnable = true;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_RESET);
    }
    // get joystick input
    int axisX = 0;
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET) {
        axisX = 1;
        this->printf("Left\r\n");
    }
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) {
        axisX = -1;
        this->printf("Right\r\n");
    }
    int axisY = 0;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET) {
        axisY = 1;
        this->printf("Up\r\n");
    }
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET) {
        axisY = -1;
        this->printf("Down\r\n");
    }

    // change stepper direction
    if(axisX != m_axisX) {
        m_axisX = axisX;
        if(m_axisX != 0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, m_axisX >= 0 ? GPIO_PIN_SET:GPIO_PIN_RESET);
    }

    if(axisY != m_axisY) {
        m_axisY = axisY;
        if(m_axisY != 0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, m_axisY >= 0 ? GPIO_PIN_SET:GPIO_PIN_RESET);
    }
    bool canMoveX = false;
    bool canMoveY = false;
    // move gantry
#ifdef QT_SIMULATOR
    if(m_axisXStep+m_axisX >= 0 &&
                m_axisXStep+m_axisX <= m_axisXStepMax) {
            m_axisXStep += m_axisX;
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);

#else
    if((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET && m_axisX == 1) ||
        (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET && m_axisX == -1)
            ){
        canMoveX = true;
#endif
    }
#ifdef QT_SIMULATOR
    if(m_axisYStep+m_axisY >= 0 &&
            m_axisYStep+m_axisY <= m_axisYStepMax) {
        m_axisYStep += m_axisY;
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);
#else
    if((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET && m_axisY == -1) ||
        (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET && m_axisY == 1)
        ){
        canMoveY = true;
#endif
    }
    if(canMoveX || canMoveY) {
        if(canMoveX) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        if(canMoveY) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        delayMicrosec(500);
        if(canMoveX) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        if(canMoveY) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        delayMicrosec(500);
    } else {
        HAL_Delay(1);
    }
#ifdef QT_SIMULATOR
    this->printf("AxisX(%d/%d) AxisY(%d/%d)\r\n",
           m_axisXStep,m_axisXStepMax,
           m_axisYStep,m_axisYStepMax);
#endif
    // get claw input
    uint32_t millis = HAL_GetTick();
    if(millis - m_clawPressTime >= 2000) {
        bool clawPressed = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET;
        if(!clawPressed && m_clawPressed) {
            m_clawPressed = false;
        } else if(clawPressed && !m_clawPressed) {
            m_clawPressed = true;
            m_axisZ = 0;
            setMachineState(MACHINE_STATE::DROP_CLAW);
        }
    }
}

void ApplicationController::dropClaw() {
    if(!m_axisZEnable) {
        m_axisZEnable = true;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,GPIO_PIN_RESET);
    }
    if(m_axisZ!= 1) {
        m_axisZ = 1;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    }
    if(m_axisZStep + m_axisZ <= m_axisZStepMax) {
        m_axisZStep += m_axisZ;
#ifdef QT_SIMULATOR
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);
#else
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        delayMicrosec(500);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
        delayMicrosec(500);
#endif
    } else {
        HAL_Delay(1);
        m_axisZ = 0;
        m_claw = 0;
        setMachineState(MACHINE_STATE::CLOSE_CLAW);
    }
}
void ApplicationController::closeClaw() {
    if(m_claw != 1) {
        m_claw = 1;
    }
    if(m_clawStep + m_claw <= m_clawStepMax) {
        m_clawStep += m_claw;
#ifdef QT_SIMULATOR
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);
#else
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
#endif
    } else {
        setMachineState(MACHINE_STATE::RETURN_CLAW);
    }
    delayMicrosec(400);
}
void ApplicationController::returnClaw() {
    if(m_axisZ != -1) {
        m_axisZ = -1;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    }
    if(m_axisZStep + m_axisZ >= 0) {
        m_axisZStep += m_axisZ;
#ifdef QT_SIMULATOR
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);
#else
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        delayMicrosec(400);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
        delayMicrosec(400);
#endif
    } else {
        HAL_Delay(1);
        if(m_axisZEnable) {
            m_axisZEnable = false;
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,GPIO_PIN_SET);
        }
        setMachineState(MACHINE_STATE::MOVE_HOME);
    }


}
void ApplicationController::moveHome() {
    if(m_axisX != 1) {
        m_axisX = 1;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, m_axisX >= 0 ? GPIO_PIN_SET:GPIO_PIN_RESET);
    }
    if(m_axisY != -1) {
        m_axisY = -1;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, m_axisY >= 0 ? GPIO_PIN_SET:GPIO_PIN_RESET);
    }

    bool canMoveX = false;
    bool canMoveY = false;
#ifdef QT_SIMULATOR
    if(m_axisXStep + m_axisX >= 0) {
            m_axisXStep += m_axisX;
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);

#else
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET) {

#endif
        canMoveX = true;
    }

#ifdef QT_SIMULATOR
    if(m_axisYStep + m_axisY >= 0) {
        m_axisYStep += m_axisY;
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);
#else
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET) {

#endif
        canMoveY = true;
    }
    if(canMoveX || canMoveY) {
        if(canMoveX) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        if(canMoveY) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        delayMicrosec(800);
        if(canMoveX) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        if(canMoveY) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        delayMicrosec(800);
    }
    else HAL_Delay(1);
#ifdef QT_SIMULATOR
    if(m_axisXStep <= 0 && m_axisYStep <= 0) {
        m_claw = -1;
        setMachineState(MACHINE_STATE::DROP_PRIZE);
    }
#else
    if(!canMoveX && !canMoveY) {
        m_claw = -1;
        setMachineState(MACHINE_STATE::DROP_PRIZE);
    }
#endif
}
void ApplicationController::dropPrize() {
    if(m_clawStep + m_claw >= 0) {
        m_clawStep += m_claw;
#ifdef QT_SIMULATOR
        Q_EMIT actuatorStepChanged(
                (float)m_axisXStep/(float)m_axisXStepMax,
                (float)m_axisYStep/(float)m_axisYStepMax,
                (float)m_axisZStep/(float)m_axisZStepMax,
                (float)m_clawStep/(float)m_clawStepMax);
#endif
    } else {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
        setMachineState(MACHINE_STATE::WAIT_INPUT);
        resetGame();
    }
    delayMicrosec(400);
}
void ApplicationController::setMachineState(MACHINE_STATE newState) {
    if(newState == m_machineState) return;
    m_machineState = newState;
    switch(m_machineState) {
        case MACHINE_STATE::WAIT_INPUT:
            this->printf("WAIT_INPUT\r\n");
            break;
        case MACHINE_STATE::MOVE_GANTRY:
            this->printf("MOVE_GANTRY\r\n");
            break;
        case MACHINE_STATE::DROP_CLAW:
            this->printf("DROP_CLAW\r\n");
            break;
        case MACHINE_STATE::CLOSE_CLAW:
            this->printf("CLOSE_CLAW\r\n");
            break;
        case MACHINE_STATE::RETURN_CLAW:
            this->printf("RETURN_CLAW\r\n");
            break;
        case MACHINE_STATE::MOVE_HOME:
            this->printf("MOVE_HOME\r\n");
            break;
        case MACHINE_STATE::DROP_PRIZE:
            this->printf("DROP_PRIZE\r\n");
            break;
    }
}
