#include "MainProcess.h"
#include <unistd.h>
MainProcess::MainProcess(QThread *parent) :
    QThread(parent),
    m_thread(nullptr),
    m_stopped(false)

{
    m_application = new ApplicationSim(this);
//    m_thread = new QThread();
//    connect(m_thread,&QThread::started,this,&MainProcess::doWork);
//    this->moveToThread(m_thread);
}

MainProcess::~MainProcess()
{
    stopService();
    sleep(2);
}
void MainProcess::run() {
    Q_EMIT machineStateChanged("WAIT_INPUT");
    m_application->resetGame();
    while(!m_stopped) {
        m_application->loopFunction();
//        m_application->printf("stateMachine[%d]\r\n",m_application->m_machineState);
        m_application->msleep(30);
    }
    m_application->printf("Exit\r\n");
}
void MainProcess::startService() {
    if(m_thread != nullptr)
        m_thread->start();
    else start();
}
void MainProcess::stopService() {
    m_stopped = true;
    m_thread->terminate();
}
void MainProcess::handleCoinSignal(bool highSignal){
    m_application->m_coinSignalHigh = highSignal;
}
void MainProcess::handleButtonSignal(bool highSignal){
    m_application->m_buttonSignalHigh = highSignal;
}
void MainProcess::handleAxisChanged(int axisX, int axisY) {
    for(int i=0; i< JOYSTICK_DIR::JS_TOTAL_DIR; i++) {
        m_application->m_jsDirPressed[i] = false;
    }
    if(axisX > 0) {
        m_application->m_jsDirPressed[JOYSTICK_DIR::JS_RIGHT] = true;
    } else if(axisX < 0) {
        m_application->m_jsDirPressed[JOYSTICK_DIR::JS_LEFT] = true;
    }
    if(axisY > 0) {
        m_application->m_jsDirPressed[JOYSTICK_DIR::JS_UP] = true;
    } else if(axisY < 0) {
        m_application->m_jsDirPressed[JOYSTICK_DIR::JS_DOWN] = true;
    }
}
