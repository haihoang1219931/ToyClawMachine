#include "ApplicationSim.h"
#include <sys/time.h>
#include <unistd.h>
#include "MainProcess.h"
ApplicationSim::ApplicationSim(MainProcess* mainProcess):
    m_coinSignalHigh(false),
    m_coinSignalHighTime(0),
    m_coinSignalLowTime(0),
    m_coinSignalHandling(false),
    m_buttonSignalHigh(false),
    m_buttonSignalHighTime(0),
    m_buttonSignalLowTime(0),
    m_buttonSignalHandling(false),
    m_mainProcess(mainProcess)
{
    m_player = new QMediaPlayer;
    m_player->setVolume(100);
    m_player->setPlaybackRate(1);

    m_machineState = MACHINE_STATE::WAIT_INPUT;
    for(int i=0; i< JOYSTICK_DIR::JS_TOTAL_DIR; i++) {
        m_jsDirPressed[i] = false;
    }
    m_axisValue[AXIS_X] = 0;
    m_axisValue[AXIS_Y] = 0;
    m_axisValue[AXIS_Z] = 0;
    m_axisValueMin[AXIS_X] = 0;
    m_axisValueMin[AXIS_Y] = 0;
    m_axisValueMin[AXIS_Z] = 0;
    m_axisValueMax[AXIS_X] = 50;
    m_axisValueMax[AXIS_Y] = 50;
    m_axisValueMax[AXIS_Z] = 40;
    m_clawValue = 0;
    m_clawValueMin = 0;
    m_clawValueMax = 100;
    m_soundList[SOUND_ID::SOUND_BGM] = "/Data/2024/ToyClawMachine/ToyClawMachine/qml/sound/Winner.mp3";
    m_soundList[SOUND_ID::SOUND_NEWCOIN] = "/Data/2024/ToyClawMachine/ToyClawMachine/qml/sound/1.wav";
    m_soundList[SOUND_ID::SOUND_CLAW_DROP] = "/Data/2024/ToyClawMachine/ToyClawMachine/qml/sound/2.wav";
    m_soundList[SOUND_ID::SOUND_CLAW_HIT_TOP] = "/Data/2024/ToyClawMachine/ToyClawMachine/qml/sound/3.wav";
    m_soundList[SOUND_ID::SOUND_CLAW_UP] = "/Data/2024/ToyClawMachine/ToyClawMachine/qml/sound/4.wav";
    m_soundList[SOUND_ID::SOUND_CLAW_WIN] = "/Data/2024/ToyClawMachine/ToyClawMachine/qml/sound/6.wav";
}
ApplicationSim::~ApplicationSim()
{

}
int ApplicationSim::printf(const char *fmt, ...) {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    int rc = vsprintf(buffer, fmt, args);
    va_end(args);
    ::printf("%s",buffer);
    return rc;
}
void ApplicationSim::msleep(int millis) {
#ifdef __linux__
    //linux code goes here
    struct timespec ts = { millis / 1000, (millis % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#elif _WIN32
    // windows code goes here
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#else
#endif
}

void ApplicationSim::enableAxis(MACHINE_AXIS axis, bool enable){

}

bool ApplicationSim::isNewCoinInserted() {
    if(m_coinSignalHigh && !m_coinSignalHandling) {
        struct timeval curTime;
        gettimeofday(&curTime, NULL);
        m_coinSignalHighTime = curTime.tv_usec/1000 + curTime.tv_sec*1000;
        m_coinSignalHandling = true;
    }
    else if(!m_coinSignalHigh && m_coinSignalHandling)
    {
        struct timeval curTime;
        gettimeofday(&curTime, NULL);
        m_coinSignalLowTime = curTime.tv_usec/1000 + curTime.tv_sec*1000;
        m_coinSignalHandling = false;
        if(m_coinSignalLowTime - m_coinSignalHighTime >= 100) {
            printf("====== Coin signal [%ld]ms\r\n",
                   (m_coinSignalLowTime - m_coinSignalHighTime));
            return true;
        }
    }
    return false;
}
void ApplicationSim::updateCoin(int coin) {
    m_coin = coin;
    Q_EMIT m_mainProcess->coinChanged(m_coin);
}

bool ApplicationSim::isButtonPressed() {
    if(m_buttonSignalHigh && !m_buttonSignalHandling) {
        struct timeval curTime;
        gettimeofday(&curTime, NULL);
        m_buttonSignalHighTime = curTime.tv_usec/1000 + curTime.tv_sec*1000;
        m_buttonSignalHandling = true;
    }
    else if(!m_buttonSignalHigh && m_buttonSignalHandling)
    {
        struct timeval curTime;
        gettimeofday(&curTime, NULL);
        m_buttonSignalLowTime = curTime.tv_usec/1000 + curTime.tv_sec*1000;
        m_buttonSignalHandling = false;
        if(m_buttonSignalLowTime - m_buttonSignalHighTime >= 1) {
            printf("====== button signal [%ld]ms\r\n",
                   (m_buttonSignalLowTime - m_buttonSignalHighTime));
            return true;
        }
    }
    return false;
}

bool ApplicationSim::isJoystickPressed(JOYSTICK_DIR dir) {
    return m_jsDirPressed[dir];
}

void ApplicationSim::setAxisDir(MACHINE_AXIS axis, int dir) {
    m_axisDir[axis] = dir;
}

bool ApplicationSim::isAxisAtMin(MACHINE_AXIS axis) {
    return m_axisValue[axis] <= m_axisValueMin[axis];
}

bool ApplicationSim::isAxisAtMax(MACHINE_AXIS axis) {
    return m_axisValue[axis] >= m_axisValueMax[axis];
}

void ApplicationSim::moveAxis(MACHINE_AXIS axis) {
    m_axisValue[axis]+=m_axisDir[axis];
    Q_EMIT m_mainProcess->actuatorStepChanged(
            (float)m_axisValue[MACHINE_AXIS::AXIS_X]/(float)m_axisValueMax[MACHINE_AXIS::AXIS_X],
            (float)m_axisValue[MACHINE_AXIS::AXIS_Y]/(float)m_axisValueMax[MACHINE_AXIS::AXIS_Y],
            (float)m_axisValue[MACHINE_AXIS::AXIS_Z]/(float)m_axisValueMax[MACHINE_AXIS::AXIS_Z],
            (float)m_clawValue/(float)m_clawValueMax);
}

void ApplicationSim::activeClaw(bool active) {
    m_clawEnable = active;
    m_clawDir = active?1:-1;
    m_clawValue += m_clawDir;
    Q_EMIT m_mainProcess->actuatorStepChanged(
            (float)m_axisValue[MACHINE_AXIS::AXIS_X]/(float)m_axisValueMax[MACHINE_AXIS::AXIS_X],
            (float)m_axisValue[MACHINE_AXIS::AXIS_Y]/(float)m_axisValueMax[MACHINE_AXIS::AXIS_Y],
            (float)m_axisValue[MACHINE_AXIS::AXIS_Z]/(float)m_axisValueMax[MACHINE_AXIS::AXIS_Z],
            (float)m_clawValue/(float)m_clawValueMax);
}

bool ApplicationSim::isClawAtMin() {
    return m_clawValue <= m_clawValueMin;
}

bool ApplicationSim::isClawAtMax() {
    return m_clawValue >= m_clawValueMax;
}
void ApplicationSim::activeJoystickLED(bool active) {
    Q_EMIT m_mainProcess->ledJoystickChanged(active);
}
void ApplicationSim::activeButtonLED(bool active) {
    Q_EMIT m_mainProcess->ledButtonChanged(active);
}
void ApplicationSim::showStateMachine(MACHINE_STATE newState) {
    switch(m_machineState) {
    case MACHINE_STATE::WAIT_INPUT:
        Q_EMIT m_mainProcess->machineStateChanged("WAIT_INPUT");
        break;
    case MACHINE_STATE::WAIT_READY:
        Q_EMIT m_mainProcess->machineStateChanged("WAIT_READY");
        break;
    case MACHINE_STATE::MOVE_GANTRY:
        Q_EMIT m_mainProcess->machineStateChanged("MOVE_GANTRY");
        break;
    case MACHINE_STATE::DROP_CLAW:
        Q_EMIT m_mainProcess->machineStateChanged("DROP_CLAW");
        break;
    case MACHINE_STATE::CLOSE_CLAW:
        Q_EMIT m_mainProcess->machineStateChanged("CLOSE_CLAW");
        break;
    case MACHINE_STATE::RETURN_CLAW:
        Q_EMIT m_mainProcess->machineStateChanged("RETURN_CLAW");
        break;
    case MACHINE_STATE::MOVE_HOME:
        Q_EMIT m_mainProcess->machineStateChanged("MOVE_HOME");
        break;
    case MACHINE_STATE::OPEN_CLAW:
        Q_EMIT m_mainProcess->machineStateChanged("OPEN_CLAW");
        break;
    }
}

void ApplicationSim::playSound(SOUND_ID soundID, bool repeat) {
    QUrl curUrl =  m_player->media().canonicalUrl();
    QUrl newUrl= QUrl::fromLocalFile(m_soundList[soundID]);
    if(m_player->state() != QMediaPlayer::State::PlayingState) {
        m_player->setMedia(newUrl);
        m_player->play();
    }
    if(newUrl!=curUrl){
        if(m_player->state() == QMediaPlayer::State::PlayingState) {
            m_player->stop();
            m_player->setMedia(newUrl);
            m_player->play();
        }
    } else {
        if(repeat && m_player->state() == QMediaPlayer::State::StoppedState) {
            m_player->play();
        }
    }
}
void ApplicationSim::pauseSound() {

}

void ApplicationSim::continueSound(SOUND_ID soundID, int position, bool repeat){
    QUrl curUrl =  m_player->media().canonicalUrl();
    QUrl newUrl= QUrl::fromLocalFile(m_soundList[soundID]);
    if(curUrl == newUrl) {
        if(m_player->state() != QMediaPlayer::State::PlayingState) m_player->play();
        return;
    }
    printf("continueSound newUrl[%s] pos[%d]\r\n",
           newUrl.toString().toStdString().c_str(),position);
    m_player->setMedia(QUrl::fromLocalFile(m_soundList[soundID]));
    m_player->setPosition(position);
    m_player->play();
}
bool ApplicationSim::soundStopped(SOUND_ID soundID) {
    QUrl curUrl =  m_player->media().canonicalUrl();
    QUrl newUrl= QUrl::fromLocalFile(m_soundList[soundID]);
    if(curUrl == newUrl)
    return m_player->state() == QMediaPlayer::State::StoppedState;
    else return true;
}
int ApplicationSim::getSoundPosition(SOUND_ID soundID) {
    QUrl curUrl =  m_player->media().canonicalUrl();
    QUrl newUrl= QUrl::fromLocalFile(m_soundList[soundID]);
    if(curUrl == newUrl)
        return m_player->position();
    else
        return 0;
}
