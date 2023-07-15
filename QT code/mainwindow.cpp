#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comserial.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

      devserial = new QSerialPort(this);

      /* Create Object the Class comserial to manipulate read/write of the my way */
      procSerial = new comserial(devserial);


          serialBuffer = "";
          parsed_data = "";
          temperature_value = "";
          humidity_value ="";


      QStringList DispSeriais = procSerial->ConnectDevices();
      ui->comboBoxPort->addItems(DispSeriais);


      if(DispSeriais.length() > 0) {
          ui->pushButtonConnect->setEnabled(true);
          ui->textEditGetData->append("###  Cổng nối tiếp đã sẵn sàng để sử dụng.");
      }
      else { ui->textEditGetData->append("### Không phát hiện thấy cổng nối tiếp!");
      }





      connect(devserial, SIGNAL(readyRead()), this, SLOT(readSerial()));


  }




MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButtonSendData_clicked()
{

    QString Cmd = ui->lineEditCmd->text()+ "\n";
    qDebug() << "Output: " << Cmd << endl;
    WriteData(Cmd.toUtf8());

}

void MainWindow::ReadData() {
    QByteArray data = devserial->readLine();
    QString receivedData = QString::fromUtf8(data); // Chuyển đổi từ QByteArray sang QString sử dụng mã hóa UTF-8


//  QString receivedData = procSerial->Read();

    qDebug() << "input: " << receivedData << endl;
    ui->textEditGetData->append(receivedData);

    qDebug() << "ReadData() is called when data is available.";

    //Kiểm tra dữ liệu từ cổng Serial
   qDebug() << "Serial data received: " << procSerial->Read();
   updateTemperature(receivedData);

}

void MainWindow::WriteData(const QByteArray data)
{
    procSerial->Write(data);
}

void MainWindow::on_pushButtonConnect_clicked()
{   bool statusOpenSerial;
    statusOpenSerial = procSerial->ConnectCom(ui->comboBoxPort->currentText());
    if(statusOpenSerial){
        ui->textEditGetData->append("Ket noi port thanh cong");
    }
    else {
        ui->textEditGetData->append("ket noi that bai");
    }
}


void MainWindow::on_pushButtonDisconnect_clicked()
{

        bool statusCloseSerial;


        statusCloseSerial = procSerial->Disconnect();


        if (statusCloseSerial) {

            ui->textEditGetData->append("### NGat ket noi thanh cong!");
        }
        else {
            ui->textEditGetData->append("###Ngawt ket noi that bai.");
        }

}

void MainWindow::updateTemperature(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberTemperature->display(sensor_reading);
}
void MainWindow::updateHumidity(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberHumidity->display(sensor_reading);
}
void MainWindow::dayHatch(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberDayHatch->display(sensor_reading);
}
void MainWindow::hourHatch(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberHourHatch->display(sensor_reading);
}
void MainWindow::minHatch(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberMinHatch->display(sensor_reading);
}

/*
void MainWindow::readSerial()
{

    QStringList buffer_split = serialBuffer.split("\r\n"); //  split the serialBuffer string, parsing with '\r\n' as the separator

    if(buffer_split.length() < 3){

        serialData = devserial->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        qDebug() << serialBuffer;
        serialData.clear();
    }else{

        serialBuffer = "";
        qDebug() << buffer_split << "\n";
        temperature_value = buffer_split[0];
        humidity_value = buffer_split[1];

        qDebug() << "Temperature: " <<  temperature_value << "\n";
        qDebug() << "Humidity: " <<  humidity_value << "\n";


        MainWindow::updateTemperature(temperature_value);
        MainWindow::updateHumidity(humidity_value);


    }

}

*/
void MainWindow::readSerial()
{
    QByteArray serialData = devserial->readAll(); // Đọc dữ liệu từ cổng serial
    serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
    qDebug() << "dữ liệu nhận được: " << serialBuffer;
// Kiểm tra điều kiện kết thúc
    if (serialBuffer.endsWith("\n")) {
        // Xử lý dữ liệu đã đọc
        QStringList values = serialBuffer.split(',');

        if (values.size() >=2 ) {
            QString temperature1 = values[0];
            QString humidity1 = values[1];

/*
            if (values.size() >= 4) {
                QString temperature2 = values[2];
                QString humidity2 = values[3];
                MainWindow::updateTemperature(temperature2);
                MainWindow::updateHumidity(humidity2);
            }
*/
            if (values.size() >= 5) {
                QString value1 = values[2];
                QString value2 = values[3];
                QString value3 = values[4];
                MainWindow::dayHatch(value1);
                MainWindow::hourHatch(value2);
                MainWindow::minHatch(value3);


            }
            MainWindow::updateTemperature(temperature1);
            MainWindow::updateHumidity(humidity1);


        } else {
            // Xử lý lỗi hoặc thông báo không đủ phần tử trong chuỗi đầu vào
            qDebug() << "Lỗi: Không đủ phần tử trong chuỗi đầu vào.";
        }


    // Xóa serialBuffer để chuẩn bị cho lần đọc tiếp theo
    serialBuffer.clear();
}
}
void MainWindow::on_pushButtonTurnOn_clicked()
{
    if (devserial->isOpen() && devserial->isWritable()) {
        // Gửi dữ liệu xuống Arduino
        QString dataToSend = "TurnOnLed";
        QByteArray sendData = dataToSend.toUtf8(); // Chuyển đổi QString sang QByteArray
        qint64 bytesWritten = devserial->write(sendData);
        qDebug()<< "Send Data: "<<sendData;
        if (bytesWritten == -1) {
            // Xử lý lỗi: Không gửi được dữ liệu
            qDebug() << "Error writing data to serial port";
        } else if (bytesWritten < sendData.size()) {
            // Xử lý lỗi: Chưa gửi được toàn bộ dữ liệu
            qDebug() << "Not all data has been written to serial port";
        } else {
            // Gửi dữ liệu thành công
            devserial->waitForBytesWritten(100); // Đợi cho đến khi dữ liệu được gửi đi
        }
    } else {
        // Xử lý lỗi: Serial port không mở hoặc không ghi được
        qDebug() << "Serial port is not open or not writable";
    }
}


void MainWindow::on_pushButtonTurnOff_clicked()
{
    if (devserial->isOpen() && devserial->isWritable()) {
        // Gửi dữ liệu xuống Arduino
        QString dataToSend = "TurnOffLed";
        QByteArray sendData = dataToSend.toUtf8(); // Chuyển đổi QString sang QByteArray
        qint64 bytesWritten = devserial->write(sendData);
        qDebug()<< "Send Data: "<<sendData;
        if (bytesWritten == -1) {
            // Xử lý lỗi: Không gửi được dữ liệu
            qDebug() << "Error writing data to serial port";
        } else if (bytesWritten < sendData.size()) {
            // Xử lý lỗi: Chưa gửi được toàn bộ dữ liệu
            qDebug() << "Not all data has been written to serial port";
        } else {
            // Gửi dữ liệu thành công
            devserial->waitForBytesWritten(100); // Đợi cho đến khi dữ liệu được gửi đi
        }
    } else {
        // Xử lý lỗi: Serial port không mở hoặc không ghi được
        qDebug() << "Serial port is not open or not writable";
    }
}




void MainWindow::on_pushButtonTurnOffSpeaker_clicked()
{
    if (devserial->isOpen() && devserial->isWritable()) {
        // Gửi dữ liệu xuống Arduino
        QString dataToSend = "TurnOffSpeaker";
        QByteArray sendData = dataToSend.toUtf8(); // Chuyển đổi QString sang QByteArray
        qint64 bytesWritten = devserial->write(sendData);
        qDebug()<< "Send Data: "<<sendData;
        if (bytesWritten == -1) {
            // Xử lý lỗi: Không gửi được dữ liệu
            qDebug() << "Error writing data to serial port";
        } else if (bytesWritten < sendData.size()) {
            // Xử lý lỗi: Chưa gửi được toàn bộ dữ liệu
            qDebug() << "Not all data has been written to serial port";
        } else {
            // Gửi dữ liệu thành công
            devserial->waitForBytesWritten(100); // Đợi cho đến khi dữ liệu được gửi đi
        }
    } else {
        // Xử lý lỗi: Serial port không mở hoặc không ghi được
        qDebug() << "Serial port is not open or not writable";
    }
}




void MainWindow::on_pushButtonStartHatch_clicked()
{
    procSerial->comserial::sendDataToPort(ui->spinBoxDayTimeHatch->value(),"dayTimeToHatch");
    procSerial->comserial::sendDataToPort(ui->spinBoxHourTimeHatch->value(),"hourTimeToHatch");
    procSerial->comserial::sendDataToPort(ui->spinBoxMinuteTimeHatch->value(),"minuteTimeToHatch");
    procSerial->comserial::sendDataToPort(1,"prState");

}

void MainWindow::on_pushButtonStopHatch_clicked()
{
    procSerial->comserial::sendDataToPort(0,"prState");
}


void MainWindow::on_pushButtonContinue_clicked()
{
    procSerial->comserial::sendDataToPort(1,"prState");

}


void MainWindow::on_pushButtonSetTemp_clicked()
{
    procSerial->comserial::sendDataToPort(ui->spinBoxSetTemp->value(),"setTemp");

}


void MainWindow::on_pushButtonTurnOffBuzzer_clicked()
{
    procSerial->comserial::sendDataToPort(2,"TurnOffBuzzer");
}

