#include "ApplicationController.h"

ApplicationController::ApplicationController(QThread *parent) : QThread(parent)
{
    m_coin = 0;
    m_maxTime = 60;
    setMachineState(MACHINE_STATE::WAIT_INPUT);
    resetGame();
}

ApplicationController::~ApplicationController() {
    stopService();
    sleep(2);
}
void ApplicationController::resetGame() {
    m_timeElapsed = m_maxTime;
    m_axisX = 0;
    m_axisY = 0;
    m_axisZ = 0;
    
    m_axisXStep = 0;
    m_axisYStep = 0;
    m_axisZStep = 0;
    m_clawStep = 0;
    
    m_axisXStepMax = 60;
    m_axisYStepMax = 50;
    m_axisZStepMax = 40;
    m_clawStepMax = 10;
}

void ApplicationController::startService() {
    m_stopped = false;
    start();
}

void ApplicationController::stopService() {
    m_stopped = true;
}

void ApplicationController::handleCoinPushed() {
    m_coin ++;
    Q_EMIT coinChanged(m_coin);
}
void ApplicationController::handleAxisChanged(int axisX, int axisY) {
    m_axisX = axisX;
    m_axisY = axisY;
}
void ApplicationController::handleClawPressed() {
    m_clawPressed = true;
}
int ApplicationController::getTimeElapsed() {
    return m_timeElapsed;
}

void ApplicationController::run() {
    Q_EMIT machineStateChanged("WAIT_INPUT");
    resetGame();
    while(!m_stopped) {
        
        switch(m_machineState) {
        case MACHINE_STATE::WAIT_INPUT:
            waitInput();
            break;
        case MACHINE_STATE::WAIT_START:
            waitStart();
            break;
        case MACHINE_STATE::MOVE_GANTRY:
            moveGantry();
            break;
        case MACHINE_STATE::OPEN_CLAW:
            openClaw();
            break;
        case MACHINE_STATE::DROP_CLAW:
            dropClaw();
            break;
        case MACHINE_STATE::CLOSE_CLAW:
            closeClaw();
            break;
        case MACHINE_STATE::RETURN_CLAW:
            returnClaw();
            break;
        case MACHINE_STATE::MOVE_HOME:
            moveHome();
            break;
        case MACHINE_STATE::DROP_PRIZE:
            dropPrize();
            break;
        }
        printf("stateMachine[%d]\r\n",m_machineState);
        msleep(1000);
    }
    printf("Exit\r\n");
}

void ApplicationController::waitInput() {
    m_clawPressed = false;
    if(m_coin >= 1) {
        Q_EMIT coinChanged(m_coin);
        setMachineState(MACHINE_STATE::WAIT_START);
    }
}

void ApplicationController::waitStart() {
    if(m_clawPressed) {
        setMachineState(MACHINE_STATE::MOVE_GANTRY);
        m_coin --;
        Q_EMIT coinChanged(m_coin);
        m_clawPressed = false;
    }
}

void ApplicationController::moveGantry() {
    if(m_axisXStep+m_axisX >= 0 && 
            m_axisXStep+m_axisX <= m_axisXStepMax) {
        m_axisXStep += m_axisX;
    }
    if(m_axisYStep+m_axisY >= 0 && 
            m_axisYStep+m_axisY <= m_axisYStepMax) {
        m_axisYStep += m_axisY; 
    }
    printf("AxisX(%d/%d) AxisY(%d/%d)\r\n",
           m_axisXStep,m_axisXStepMax,
           m_axisYStep,m_axisYStepMax);
    if(m_clawPressed) {
        m_claw = 1;
        setMachineState(MACHINE_STATE::OPEN_CLAW);
        m_clawPressed = false;
    }
}

void ApplicationController::openClaw() {
    if(m_clawStep + m_claw <= m_clawStepMax) {
        m_clawStep += m_claw;
    } else {
        m_axisZ = 1;
        setMachineState(MACHINE_STATE::DROP_CLAW);
    }
}
void ApplicationController::dropClaw() {
    if(m_axisZStep + m_axisZ <= m_axisZStepMax) {
        m_axisZStep += m_axisZ;
    } else {
        m_axisZ = -1;
        m_claw = -1;
        setMachineState(MACHINE_STATE::CLOSE_CLAW);
    }
}
void ApplicationController::closeClaw() {
    if(m_clawStep + m_claw >= 0) {
        m_clawStep += m_claw;
    } else {
        m_axisX = -1;
        m_axisY = -1;
        setMachineState(MACHINE_STATE::RETURN_CLAW);
    }
}
void ApplicationController::returnClaw() {
    if(m_axisXStep > 0) {
        m_axisXStep += m_axisX;
    }
    if(m_axisYStep > 0) {
        m_axisYStep += m_axisY; 
    }
    if(m_axisXStep == 0 && m_axisYStep == 0) {
        setMachineState(MACHINE_STATE::MOVE_HOME);
    }
}
void ApplicationController::moveHome() {
    if(m_axisZStep > 0) {
        m_axisZStep += m_axisX;
    }
    if(m_axisYStep > 0) {
        m_axisYStep += m_axisY; 
    }
    if(m_axisXStep == 0 && m_axisYStep == 0) {
        m_claw = 1;
        setMachineState(MACHINE_STATE::DROP_PRIZE);
    }
}
void ApplicationController::dropPrize() {
    if(m_clawStep + m_claw <= m_clawStepMax) {
        m_clawStep += m_claw;
    } else {
        setMachineState(MACHINE_STATE::WAIT_INPUT);
        resetGame();
    }
}
void ApplicationController::setMachineState(MACHINE_STATE newState) {
    if(newState == m_machineState) return;
    m_machineState = newState;
    switch(m_machineState) {
    case MACHINE_STATE::WAIT_INPUT:
        Q_EMIT machineStateChanged("WAIT_INPUT");
        break;
    case MACHINE_STATE::WAIT_START:
        Q_EMIT machineStateChanged("WAIT_START");
        break;
    case MACHINE_STATE::MOVE_GANTRY:
        Q_EMIT machineStateChanged("MOVE_GANTRY");
        break;
    case MACHINE_STATE::OPEN_CLAW:
        Q_EMIT machineStateChanged("OPEN_CLAW");
        break;
    case MACHINE_STATE::DROP_CLAW:
        Q_EMIT machineStateChanged("DROP_CLAW");
        break;
    case MACHINE_STATE::CLOSE_CLAW:
        Q_EMIT machineStateChanged("CLOSE_CLAW");
        break;
    case MACHINE_STATE::RETURN_CLAW:
        Q_EMIT machineStateChanged("RETURN_CLAW");
        break;
    case MACHINE_STATE::MOVE_HOME:
        Q_EMIT machineStateChanged("MOVE_HOME");
        break;
    case MACHINE_STATE::DROP_PRIZE:
        Q_EMIT machineStateChanged("DROP_PRIZE");
        break;
    }
}