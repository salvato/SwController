#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include "pigpiod_if2.h"// http://abyz.me.uk/rpi/pigpio/pdif2.html

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    int gpioHostHandle;

private:
    int callBackId;
    QFile *logFile;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
