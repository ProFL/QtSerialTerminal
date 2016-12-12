#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(connectSerial()));

    serialUpdate.stop();
    connect(&serialUpdate, SIGNAL(timeout()), this, SLOT(readSerial()));

    availablePortsUpdate.stop();
    availablePortsUpdate.setInterval(1000);
    connect(&availablePortsUpdate, SIGNAL(timeout()), this, SLOT(fillPorts()));
    availablePortsUpdate.start();

    connect(ui->automaticScrollCB, SIGNAL(stateChanged(int)), this, SLOT(autoScroll()));
    connect(ui->disconnectButton, SIGNAL(clicked()), this, SLOT(disconnectSerial()));

    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud1200));
    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud2400));
    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud4800));
    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud9600));
    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud19200));
    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud38400));
    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud57600));
    ui->baudRateChoice->addItem(QString::number(QSerialPort::Baud115200));

    ui->sendingTypeChoice->addItem("String");
    ui->sendingTypeChoice->addItem("Decimal");
    ui->sendingTypeChoice->addItem("Hexadecimal");
    ui->sendingTypeChoice->addItem("Binário");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::autoScroll()
{
    if (ui->automaticScrollCB->isChecked())
    {
        QTextCursor cursor = ui->incoming_TE->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->incoming_TE->setTextCursor(cursor);
    }
    else
    {
        QTextCursor cursor = ui->incoming_TE->textCursor();
        cursor.movePosition(QTextCursor::Start);
        ui->incoming_TE->setTextCursor(cursor);
    }
}

void MainWindow::fillPorts()
{
    QList<QSerialPortInfo> serialPortList = serial.getPortList();
    int n = serialPortList.size();

    ui->portChoice->clear();

    if (n == 0)
    {
        ui->portChoice->addItem("N/A");
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            ui->portChoice->addItem(serialPortList.at(i).portName());
        }
    }
}

void MainWindow::connectSerial()
{
    bool success;
    int baudRate = ui->baudRateChoice->currentText().toInt(&success);

    if (!success)
    {
        ui->incoming_TE->setText("Baud rate inválida!");
        return;
    }

    int serialPortIndex = ui->portChoice->currentIndex();

    success = serial.portConnect(serialPortIndex, baudRate);

    if (!success)
    {
        ui->incoming_TE->setText("Impossível Conectar!");
        ui->incoming_TE->append("\n");
        return;
    }

    ui->incoming_TE->setText("Connectado com sucesso.");

    if (serialUpdate.isActive())
    {
        serialUpdate.stop();
    }

    serialUpdate.setInterval(1);
    serialUpdate.start();

    int n = QSerialPortInfo::availablePorts().size();
    for (int i = 0; i < n; i++)
    {
        ui->portChoice->clear();
        ui->portChoice->addItem(QSerialPortInfo::availablePorts().at(i).portName());
    }
}

void MainWindow::disconnectSerial()
{
    if (serial.getStatus())
    {
        serialUpdate.stop();

        try
        {
            serial.portDisconnect();
        }
        catch(QString)
        {}

        ui->incoming_TE->append("Desconectado com sucesso.");
    }
}

void MainWindow::readSerial()
{
    QString message;

    try
    {
        message = serial.readLine();
    }
    catch (QString &error)
    {
        ui->incoming_TE->setText("ERRO: ");
        ui->incoming_TE->append(error);
    }

    if (message.size() > 0)
    {
        ui->incoming_TE->append(message);
    }

    if (ui->automaticScrollCB->isChecked())
    {
        QTextCursor cursor = ui->incoming_TE->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->incoming_TE->setTextCursor(cursor);
    }
}

void MainWindow::on_clearButton_clicked()
{
    ui->incoming_TE->clear();
}


void MainWindow::on_sendButton_clicked()
{
    QString textToSend = ui->sendMessage_LE->text();
    int sendType = ui->sendingTypeChoice->currentIndex();

    try
    {
        serial.sendMessage(textToSend, sendType);
    }
    catch(QString)
    {
        ui->sendMessage_LE->setText("MENSAGEM COM ERRO!");
    }
}
