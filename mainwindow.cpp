#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGlobal>
#include <QDateTime>
#include <QDebug>

#define GPIO_IN 22


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


CBFuncEx_t
gpioCallback(int pi, unsigned int user_gpio, unsigned int level, uint32_t tick, void* userdata) {
    Q_UNUSED(pi)
    Q_UNUSED(user_gpio)
    Q_UNUSED(level)
    Q_UNUSED(tick)
    ((MainWindow*)userdata)->logTime(QTime::currentTime());
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
    if(set_mode(gpioHostHandle, GPIO_IN, PI_INPUT)) {// Set the GPIO_IN as Input.
        logMessage(logFile,
                   sFunctionName,
                   QString("Non riesco ad inizializzare GPIO_IN."));
        return;
    }
    if(set_pull_up_down(gpioHostHandle, GPIO_IN, PI_PUD_DOWN)) {// Set the pull-up to GPIO_IN.
        logMessage(logFile,
                   sFunctionName,
                   QString("Non riesco ad inizializzare il pull-up in GPIO_IN."));
        return;
    }
    callBackId = callback_ex(gpioHostHandle, GPIO_IN, RISING_EDGE, (CBFuncEx_t)gpioCallback, this);
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


void
MainWindow::logTime(QTime eventTime) {
    qDebug() << eventTime;
}
