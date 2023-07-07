#include "comserial.h"
#include <QMainWindow>


comserial::comserial(QSerialPort *myDev)
{
    devSerial = myDev;

}
QStringList comserial::ConnectDevices()
{
    QStringList devs;

    foreach (const QSerialPortInfo info, QSerialPortInfo::availablePorts()) {

        devSerial->setPort(info);


        if (devSerial->open(QIODevice::ReadWrite)) {
            devSerial->close();
            devs << info.portName();
        }

    }
    return devs;
}


bool comserial::ConnectCom(QString portname){
devSerial->setPortName(portname);
   qDebug() << "Ket noi voi port: " << portname;
if (!devSerial->open(QIODevice::ReadWrite))
{
    qDebug() << "Không thể mở cổng Serial.";
    return 1;
}

// Cấu hình các thông số cho cổng Serial
devSerial->setBaudRate(QSerialPort::Baud9600);
devSerial->setDataBits(QSerialPort::Data8);
devSerial->setParity(QSerialPort::NoParity);
devSerial->setStopBits(QSerialPort::OneStop);
devSerial->setFlowControl(QSerialPort::NoFlowControl);
if (devSerial->isOpen())
   {
       qDebug() << "Đã kết nối thành công với cổng Serial " << portname;
       return true;
   }
   else
   {
       qDebug() << "Kết nối thất bại với cổng Serial " << portname;
       return false;
   }

return true;
}

bool comserial::Disconnect()
{
    devSerial->clear();
    devSerial->close();


    if(devSerial->error() == 0 || !devSerial->isOpen()) {
        qDebug() << "Dong port thanh cong";
        return true;
    }
    else {
        qDebug() << "Ngat ket noi khong thanh cong! ERRO: " << devSerial->error();
        return false;
    }

}



qint64 comserial::Write(const char *cmd)
{
    qint64 tamanhoEscrito;
    tamanhoEscrito = devSerial->write(cmd,qstrlen(cmd));

    return tamanhoEscrito;
}


QString comserial::Read()
{
     QString bufRxSerial;

    // Awaits read all the data before continuing
     while (devSerial->waitForReadyRead(1000)) {
         bufRxSerial += devSerial->readAll();
     }
     return bufRxSerial;
}

QString comserial::Read(int TamanhoBuffer)
{
    char buf[TamanhoBuffer];

    if (devSerial->canReadLine()) {
        devSerial->read(buf, sizeof(buf));
    }

    return buf;

}


void comserial::sendDataToPort( double dataToSend, QString Str){
    if (devSerial->isOpen() && devSerial->isWritable()) {
        // Đọc giá trị từ doubleSpinBox

        double dataToSendPort =  dataToSend;

         // Chuyển đổi giá trị sang QString
                QString StrdataToSend = Str + QString::number(dataToSendPort);

        // Chuyển đổi QString sang QByteArray
        QByteArray ArrSendData = StrdataToSend.toUtf8();


        // Gửi dữ liệu xuống Arduino
        qint64 bytesWritten = devSerial->write(ArrSendData);
        qDebug() << "Send Data: " << ArrSendData;

        if (bytesWritten == -1) {
            // Xử lý lỗi: Không gửi được dữ liệu
            qDebug() << "Error writing data to serial port";
        } else if (bytesWritten < ArrSendData.size()) {
            // Xử lý lỗi: Chưa gửi được
        }
    }
}
