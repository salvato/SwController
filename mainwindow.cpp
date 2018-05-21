#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGlobal>
#include <QDateTime>
#include <QDebug>


void
logMessage(QFile *logFile, QString sFunctionName, QString sMessage) {
    Q_UNUSED(sFunctionName)
    Q_UNUSED(sMessage)

    QDateTime dateTime;

    QString sDebugMessage = dateTime.currentDateTime().toString() +
                            sFunctionName +
                            sMessage;
    if(logFile) {
        if(logFile->isOpen()) {
            logFile->write(sDebugMessage.toUtf8().data());
            logFile->write("\n");
            logFile->flush();
        }
        else {
            qDebug() << sDebugMessage;
        }
    }
    else {
        qDebug() << sDebugMessage;
    }
}


CBFunc_t
gpioCallback(int pi, unsigned int user_gpio, unsigned int level, uint32_t tick) {
    Q_UNUSED(pi)
    Q_UNUSED(user_gpio)
    Q_UNUSED(level)
    Q_UNUSED(tick)
    qDebug() << "pos=";
    return 0;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , gpioHostHandle(-1)
    , logFile(Q_NULLPTR)
    , ui(new Ui::MainWindow)
{
    QString sFunctionName = " MainWindow::MainWindow ";
    Q_UNUSED(sFunctionName)

    ui->setupUi(this);
    gpioHostHandle = pigpio_start((char*)"localhost", (char*)"8888");
    if(gpioHostHandle < 0) {
        logMessage(logFile,
                   sFunctionName,
                   QString("Non riesco ad inizializzare la GPIO."));
        return;
    }

    // All GPIOs are identified by their Broadcom number.
    if(set_mode(gpioHostHandle, 23, PI_INPUT)) {// Set the GPIO23 as Input.
        logMessage(logFile,
                   sFunctionName,
                   QString("Non riesco ad inizializzare GPIO23."));
        return;
    }
    if(set_pull_up_down(gpioHostHandle, 23, PI_PUD_UP)) {// Set the pull-up to GPIO23.
        logMessage(logFile,
                   sFunctionName,
                   QString("Non riesco ad inizializzare il pull-up in GPIO23."));
        return;
    }
    callBackId = callback(gpioHostHandle, 23, FALLING_EDGE, (CBFunc_t)gpioCallback);
    if(callBackId == pigif_bad_malloc) {
        logMessage(logFile,
                   sFunctionName,
                   QString("pigif_bad_malloc"));
        return;
    }
    if(callBackId == pigif_duplicate_callback) {
        logMessage(logFile,
                   sFunctionName,
                   QString("pigif_duplicate_callback"));
        return;
    }
    if(callBackId == pigif_bad_callback) {
        logMessage(logFile,
                   sFunctionName,
                   QString("pigif_bad_callback"));
        return;
    }
    logMessage(logFile,
               sFunctionName,
               QString("Done !"));
}


MainWindow::~MainWindow() {
    callback_cancel(callBackId);
    if(gpioHostHandle >= 0) {
        pigpio_stop(gpioHostHandle);
    }
    delete ui;
}
