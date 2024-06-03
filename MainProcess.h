#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QObject>
#include <QThread>
#include "ApplicationSim.h"

class MainProcess : public QThread
{
    Q_OBJECT
public:
    explicit MainProcess(QThread *parent = nullptr);
    ~MainProcess();
Q_SIGNALS:
    void ledJoystickChanged(bool active);
    void ledButtonChanged(bool active);
    void timeElapsedChanged(QString time);
    void coinChanged(int coin);
    void machineStateChanged(QString state);
    void actuatorStepChanged(float ratioX, float ratioY, float ratioZ, float ratioClaw);
public Q_SLOTS:
    void run() override;
    void startService();
    void stopService();
    void handleCoinSignal(bool highSignal);
    void handleButtonSignal(bool highSignal);
    void handleAxisChanged(int axisX, int axisY);
private:
    QThread* m_thread;
    ApplicationSim* m_application;
    bool m_stopped;
    int m_coin;

    int m_axisDir[MACHINE_AXIS::AXIS_MAX];

    int m_clawDir;
};

#endif // MAINPROCESS_H
