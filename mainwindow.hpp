#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include "serialport.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    SerialPort serial;
    QTimer serialUpdate;
    QTimer availablePortsUpdate;

private slots:
    void autoScroll();
    void fillPorts();
    void connectSerial();
    void disconnectSerial();
    void readSerial();

    void on_clearButton_clicked();
    void on_sendButton_clicked();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_HPP
