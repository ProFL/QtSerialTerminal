#include "serialport.hpp"

SerialPort::SerialPort() : port(NULL), connected(false)
{}

bool SerialPort::portConnect(const int PORT_INDEX, const int BAUD_RATE)
{
    if (getPortList().size() == 0)
    {
        return false;
    }

    if (connected)
    {
        port->close();
        delete port;
        port = NULL;
    }

    port = new QSerialPort(getPortList().at(PORT_INDEX));
    port->setBaudRate(BAUD_RATE);

    connected = port->open(QIODevice::ReadWrite);

    return connected;
}

void SerialPort::portDisconnect()
{
    if(!connected)
    {
        throw QString("Porto já desconectado.");
    }

    port->close();
    delete port;
    port = NULL;
    connected = false;
}

QString SerialPort::readLine()
{
    if (!connected)
    {
        throw QString("Falha ao ler porto serial, não conectado.");
    }

    QString message;
    char incomingChar = '\0';

    while(port->canReadLine() && incomingChar != '\n')
    {
        port->getChar(&incomingChar);
        message += incomingChar;
    }

    return message;
}

void SerialPort::sendMessage(const QString MESSAGE, const int TYPE)
{
    bool success = true;

    if (TYPE == String)
    {
        QString outgoingMessage = MESSAGE;

        port->write(MESSAGE.toStdString().c_str());
    }
    else
    {
        unsigned long long outgoingMessage;
        if (TYPE == Decimal)
        {
            outgoingMessage = MESSAGE.toULongLong(&success, 10);
        }
        else if (TYPE == Hexadecimal)
        {
            outgoingMessage = MESSAGE.toULongLong(&success, 16);
        }
        else if (TYPE == Binary)
        {
            outgoingMessage = MESSAGE.toULongLong(&success, 2);
        }

        if (!success)
        {
            throw QString("Falha ao preparar a mensagem para envio!");
        }

        QByteArray outgoing;

        for (int i = 0; i < 8; i++)
        {
            if (((outgoingMessage >> (i * 8)) & 0xFFULL) == 0)
            {
                break;
            }

            outgoing.append(char((outgoingMessage >> (i * 8)) & 0xFFULL));
        }

        port->write(outgoing);
    }
}

bool SerialPort::getStatus()
{
    return connected;
}

QList<QSerialPortInfo> SerialPort::getPortList()
{
    return QSerialPortInfo::availablePorts();
}
