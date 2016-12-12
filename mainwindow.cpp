#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->buttonConnect, SIGNAL(clicked(bool)), this, SLOT(connectSerial()));

    serialUpdate.stop();
    connect(&serialUpdate, SIGNAL(timeout()), this, SLOT(readSerial()));

    availablePortsUpdate.stop();
    availablePortsUpdate.setInterval(1000);
    connect(&availablePortsUpdate, SIGNAL(timeout()), this, SLOT(fillPorts()));
    availablePortsUpdate.start();

    connect(ui->chkBoxAutomaticScroll, SIGNAL(stateChanged()), this, SLOT(autoScroll()));
    connect(ui->buttonDisconnect, SIGNAL(clicked()), this, SLOT(disconnectSerial()));

    connect(ui->buttonSend, SIGNAL(clicked()), this, SLOT(sendMessage()));
    connect(ui->lineEditSendMessage, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

    connect(ui->buttonClear, SIGNAL(clicked()), this, SLOT(clearIncoming()));

    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud1200));
    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud2400));
    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud4800));
    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud9600));
    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud19200));
    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud38400));
    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud57600));
    ui->cbBoxBaud->addItem(QString::number(QSerialPort::Baud115200));

    ui->cbBoxSendType->addItem("String");
    ui->cbBoxSendType->addItem("Decimal");
    ui->cbBoxSendType->addItem("Hexadecimal");
    ui->cbBoxSendType->addItem("Binário");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::autoScroll()
{
    if (ui->chkBoxAutomaticScroll->isChecked())
    {
        QTextCursor cursor = ui->textEditIncoming->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEditIncoming->setTextCursor(cursor);
    }
    else
    {
        QTextCursor cursor = ui->textEditIncoming->textCursor();
        cursor.movePosition(QTextCursor::Start);
        ui->textEditIncoming->setTextCursor(cursor);
    }
}

void MainWindow::fillPorts()
{
    QList<QSerialPortInfo> serialPortList = serial.getPortList();
    int n = serialPortList.size();

    ui->cbBoxPort->clear();

    if (n == 0)
    {
        ui->cbBoxPort->addItem("N/A");
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            ui->cbBoxPort->addItem(serialPortList.at(i).portName());
        }
    }
}

void MainWindow::connectSerial()
{
    bool success;
    int baudRate = ui->cbBoxBaud->currentText().toInt(&success);

    if (!success)
    {
        ui->textEditIncoming->setText("Baud rate inválida!");
        return;
    }

    int serialPortIndex = ui->cbBoxPort->currentIndex();

    success = serial.portConnect(serialPortIndex, baudRate);

    if (!success)
    {
        ui->textEditIncoming->setText("Impossível Conectar!");
        ui->textEditIncoming->append("\n");
        return;
    }

    ui->textEditIncoming->setText("Connectado com sucesso.");

    if (serialUpdate.isActive())
    {
        serialUpdate.stop();
    }

    serialUpdate.setInterval(1);
    serialUpdate.start();

    int n = QSerialPortInfo::availablePorts().size();
    for (int i = 0; i < n; i++)
    {
        ui->cbBoxPort->clear();
        ui->cbBoxPort->addItem(QSerialPortInfo::availablePorts().at(i).portName());
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

        ui->textEditIncoming->append("Desconectado com sucesso.");
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
        ui->textEditIncoming->setText("ERRO: ");
        ui->textEditIncoming->append(error);
    }

    if (message.size() > 0)
    {
        ui->textEditIncoming->append(message);
    }

    if (ui->chkBoxAutomaticScroll->isChecked())
    {
        QTextCursor cursor = ui->textEditIncoming->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEditIncoming->setTextCursor(cursor);
    }
}

void MainWindow::clearIncoming()
{
    ui->textEditIncoming->clear();
}


void MainWindow::sendMessage()
{
    QString textToSend = ui->lineEditSendMessage->text();
    int sendType = ui->cbBoxSendType->currentIndex();

    try
    {
        serial.sendMessage(textToSend, sendType);
    }
    catch(QString & erro)
    {
        ui->lineEditSendMessage->setText(erro);
    }
}
