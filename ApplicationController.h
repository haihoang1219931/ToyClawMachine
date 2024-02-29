#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include <QObject>
#include <QThread>
enum class MACHINE_STATE {
    WAIT_INPUT,
    WAIT_START,
    MOVE_GANTRY,
    DROP_CLAW,
    CLOSE_CLAW,
    RETURN_CLAW,
    MOVE_HOME,
    DROP_PRIZE
};

class ApplicationController : public QThread
{
    Q_OBJECT
    
public:
    explicit ApplicationController(QThread *parent = nullptr);
    virtual ~ApplicationController();
    
    void resetGame();
private:
    void waitInput();
    void waitStart();
    void moveGantry();
    void dropClaw();
    void closeClaw();
    void returnClaw();
    void moveHome();
    void dropPrize();
    void setMachineState(MACHINE_STATE newState);
public Q_SLOTS:
    void startService();
    void stopService();
    void handleCoinPushed();
    void handleAxisChanged(int axisX, int axisY);
    void handleClawPressed();
    int getTimeElapsed();
    
Q_SIGNALS:
    void timeElapsedChanged(QString time);
    void coinChanged(int coin);
    void machineStateChanged(QString state);
    void actuatorStepChanged(float ratioX, float ratioY, float ratioZ, float ratioClaw);;
//    void axisXStepChanged(int axisXStep);
//    void axisYStepChanged(int axisYStep);
//    void axisZStepChanged(int axisZStep);
//    void clawStepChanged(int clawStep);
private:
    void run() override;
    
private:
    MACHINE_STATE m_machineState;
    
    int m_timeSleep;
    bool m_stopped;
    
    int m_maxTime;
    int m_timeElapsed;
    int m_coin;
    int m_axisX;
    int m_axisY;
    int m_axisZ; 
    int m_claw;
    
    int m_axisXStep;
    int m_axisYStep;
    int m_axisZStep;
    int m_clawStep;
    
    int m_axisXStepMax;
    int m_axisYStepMax;
    int m_axisZStepMax;
    int m_clawStepMax;
    
    bool m_clawPressed;
};

#endif // APPLICATIONCONTROLLER_H
