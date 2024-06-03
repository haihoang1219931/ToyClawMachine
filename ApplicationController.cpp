#include "ApplicationController.h"

ApplicationController::ApplicationController():
    m_coin(0),
    m_joystickLEDEnable(false),
    m_buttonLEDEnable(false),
    m_bgmStopPosition(0)
{

}

ApplicationController::~ApplicationController() {
}

void ApplicationController::resetGame() {
    for(int i=0; i< MACHINE_AXIS::AXIS_MAX; i++) {
        m_axisEnable[i] = false;
        m_axisDir[i] = 0;
    }
    m_clawDir = 0;
    m_clawEnable = false;
}

void ApplicationController::loopFunction() {
	switch(m_machineState) {
		case MACHINE_STATE::WAIT_INPUT:
			waitInput();
			break;
        case MACHINE_STATE::WAIT_READY:
            waitReady();
            break;
		case MACHINE_STATE::MOVE_GANTRY:
			moveGantry();
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
        case MACHINE_STATE::OPEN_CLAW:
            openClaw();
			break;
	}
}

void ApplicationController::waitInput() {
    if(m_joystickLEDEnable) {
        m_joystickLEDEnable = false;
        activeJoystickLED(false);
    }
    if(m_coin <= 0 && m_buttonLEDEnable) {
        m_buttonLEDEnable = false;
        activeButtonLED(false);
    }
    if(m_axisEnable[AXIS_X]) {
        m_axisEnable[AXIS_X] = false;
        enableAxis(MACHINE_AXIS::AXIS_X,false);
    }

    if(m_axisEnable[AXIS_Y]) {
        m_axisEnable[AXIS_Y] = false;
        enableAxis(MACHINE_AXIS::AXIS_Y,false);
	}

    if(m_axisEnable[AXIS_Z]) {
        m_axisEnable[AXIS_Z] = false;
        enableAxis(MACHINE_AXIS::AXIS_Z,false);
	}
    if(isNewCoinInserted()) {
        updateCoin(m_coin + 1);
        m_bgmStopPosition = getSoundPosition(SOUND_ID::SOUND_BGM);
        playSound(SOUND_ID::SOUND_NEWCOIN);
        if(!m_buttonLEDEnable) {
            m_buttonLEDEnable = true;
            activeButtonLED(true);
        }
    } else {
        if(soundStopped(SOUND_ID::SOUND_NEWCOIN)){
            continueSound(SOUND_ID::SOUND_BGM,m_bgmStopPosition);
        }
    }
    if(isButtonPressed()) {
        if(m_coin > 0) updateCoin(m_coin - 1);
        setMachineState(MACHINE_STATE::WAIT_READY);
    }

}
void ApplicationController::waitReady() {
    msleep(1000);
    setMachineState(MACHINE_STATE::MOVE_GANTRY);
}
void ApplicationController::moveGantry() {
    if(!m_joystickLEDEnable) {
        m_joystickLEDEnable = true;
        activeJoystickLED(true);
    }
    if(!m_axisEnable[AXIS_X]) {
        m_axisEnable[AXIS_X] = true;
        enableAxis(MACHINE_AXIS::AXIS_X,true);
	}

    if(!m_axisEnable[AXIS_Y]) {
        m_axisEnable[AXIS_Y] = true;
        enableAxis(MACHINE_AXIS::AXIS_Y,true);
	}
	// get joystick input
    int axisXDir = 0;
    if(isJoystickPressed(JOYSTICK_DIR::JS_LEFT)) {
        axisXDir = -1;
        this->printf("Left\r\n");
	}
    if(isJoystickPressed(JOYSTICK_DIR::JS_RIGHT)) {
        axisXDir = +1;
        this->printf("Right\r\n");
	}
    int axisYDir = 0;
    if(isJoystickPressed(JOYSTICK_DIR::JS_UP)) {
        axisYDir = 1;
        this->printf("Up\r\n");
	}
    if(isJoystickPressed(JOYSTICK_DIR::JS_DOWN)) {
        axisYDir = -1;
        this->printf("Down\r\n");
	}

	// change stepper direction
    if(axisXDir != m_axisDir[AXIS_X]) {
        m_axisDir[AXIS_X] = axisXDir;
        if(m_axisDir[AXIS_X] != 0) setAxisDir(MACHINE_AXIS::AXIS_X, axisXDir);
	}

    if(axisYDir != m_axisDir[AXIS_Y]) {
        m_axisDir[AXIS_Y] = axisYDir;
        if(m_axisDir[AXIS_Y] != 0) setAxisDir(MACHINE_AXIS::AXIS_Y, axisYDir);
    }

	// move gantry
    if((!isAxisAtMin(MACHINE_AXIS::AXIS_X) && axisXDir < 0) ||
        (!isAxisAtMax(MACHINE_AXIS::AXIS_X) && axisXDir > 0)) {
        moveAxis(MACHINE_AXIS::AXIS_X);
    }
    if((!isAxisAtMin(MACHINE_AXIS::AXIS_Y) && axisYDir < 0) ||
        (!isAxisAtMax(MACHINE_AXIS::AXIS_Y) && axisYDir > 0)) {
        moveAxis(MACHINE_AXIS::AXIS_Y);
    }

    // get claw input
    if(isButtonPressed()) {
        setMachineState(MACHINE_STATE::DROP_CLAW);
    }
}

void ApplicationController::dropClaw() {
    if(!m_axisEnable[MACHINE_AXIS::AXIS_Z]) {
        m_axisEnable[MACHINE_AXIS::AXIS_Z] = true;
        enableAxis(MACHINE_AXIS::AXIS_Z,true);
	}
    if(m_axisDir[MACHINE_AXIS::AXIS_Z]!= 1) {
        m_axisDir[MACHINE_AXIS::AXIS_Z] = 1;
        setAxisDir(MACHINE_AXIS::AXIS_Z,m_axisDir[MACHINE_AXIS::AXIS_Z]);
	}
    if(!isAxisAtMax(MACHINE_AXIS::AXIS_Z)) {
        moveAxis(MACHINE_AXIS::AXIS_Z);
        m_bgmStopPosition = getSoundPosition(SOUND_ID::SOUND_BGM);
        playSound(SOUND_ID::SOUND_CLAW_DROP, true);
    } else {
        setMachineState(MACHINE_STATE::CLOSE_CLAW);
        playSound(SOUND_ID::SOUND_CLAW_HIT_TOP);
    }

}
void ApplicationController::closeClaw() {
    if(!isClawAtMax()) {
        activeClaw(true);
    } else {
        setMachineState(MACHINE_STATE::RETURN_CLAW);
    }
}
void ApplicationController::returnClaw() {
    if(!m_axisEnable[MACHINE_AXIS::AXIS_Z]) {
        m_axisEnable[MACHINE_AXIS::AXIS_Z] = true;
        enableAxis(MACHINE_AXIS::AXIS_Z,true);
    }
    if(m_axisDir[MACHINE_AXIS::AXIS_Z]!= -1) {
        m_axisDir[MACHINE_AXIS::AXIS_Z] = -1;
        setAxisDir(MACHINE_AXIS::AXIS_Z,m_axisDir[MACHINE_AXIS::AXIS_Z]);
    }
    if(!isAxisAtMin(MACHINE_AXIS::AXIS_Z)) {
        moveAxis(MACHINE_AXIS::AXIS_Z);
        playSound(SOUND_ID::SOUND_CLAW_UP, true);
    } else {
        setMachineState(MACHINE_STATE::MOVE_HOME);
        playSound(SOUND_ID::SOUND_CLAW_WIN);
    }
}
void ApplicationController::moveHome() {
    bool axisXAtMin = false;
    bool axisYAtMin = false;
    if(m_axisDir[MACHINE_AXIS::AXIS_X] != -1) {
        m_axisDir[MACHINE_AXIS::AXIS_X] = -1;
        setAxisDir(MACHINE_AXIS::AXIS_X, m_axisDir[MACHINE_AXIS::AXIS_X]);
    }
    if(m_axisDir[MACHINE_AXIS::AXIS_Y] != -1) {
        m_axisDir[MACHINE_AXIS::AXIS_Y] = -1;
        setAxisDir(MACHINE_AXIS::AXIS_Y, m_axisDir[MACHINE_AXIS::AXIS_Y]);
    }
    if(!isAxisAtMin(MACHINE_AXIS::AXIS_X)) {
        moveAxis(MACHINE_AXIS::AXIS_X);
    } else {
        axisXAtMin = true;
    }
    if(!isAxisAtMin(MACHINE_AXIS::AXIS_Y)) {
        moveAxis(MACHINE_AXIS::AXIS_Y);
    } else {
        axisYAtMin = true;
    }
    if(axisXAtMin && axisYAtMin) {
        setMachineState(MACHINE_STATE::OPEN_CLAW);
    }
}
void ApplicationController::openClaw() {
    if(!isClawAtMin()) {
        activeClaw(false);
    } else {
        setMachineState(MACHINE_STATE::WAIT_INPUT);
    }
}
void ApplicationController::setMachineState(MACHINE_STATE newState) {
    if(newState == m_machineState) return;
    m_machineState = newState;
    showStateMachine(m_machineState);
    switch(m_machineState) {
    case MACHINE_STATE::WAIT_INPUT:
        this->printf("WAIT_INPUT\r\n");
        break;
    case MACHINE_STATE::WAIT_READY:
        this->printf("WAIT_READY\r\n");
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
    case MACHINE_STATE::OPEN_CLAW:
        this->printf("OPEN_CLAW\r\n");
        break;
    }
}
