#ifndef COMSERIAL_H
#define COMSERIAL_H


/**
 * Incluindo QDebug para enviar debug em background no terminal
 * QSerialPort e QSerialPortInfo para manipular o dispositivo serial
 */
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class comserial
{
public:
    comserial(QSerialPort *myDev);
    ~comserial();

    QStringList ConnectDevices();
    bool ConnectCom(QString portname);
    bool Connect(QString Port, uint32_t bd);
    bool Disconnect(void);
    bool mySerial(QSerialPort *devSerial);
    void updateTemperature(QString sensor_reading);

    void  sendDataToPort( double dataToSend, QString Str);

    qint64 Write(const char *cmd);
    QString Read();
    QString Read(int TamanhoBuffer);

protected:
    QSerialPort *devSerial;
};

#endif // COMSERIAL_H
