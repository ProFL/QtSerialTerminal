#ifndef SERIALPORT_HPP
#define SERIALPORT_HPP

#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPort
{
private:
    QSerialPort * port;
    bool connected;

public:
    enum MessageType
    {
        String = 0,
        Decimal,
        Hexadecimal,
        Binary
    };

    SerialPort();

    bool portConnect(const int PORT_INDEX, const int BAUD_RATE);
    void portDisconnect();

    QString readLine();
    void sendMessage(const QString MESSAGE, const int TYPE);

    bool getStatus();

    static QList<QSerialPortInfo> getPortList();
};

#endif // SERIALPORT_HPP
