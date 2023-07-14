#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "comserial.h"
#include <QDebug>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSendData_clicked();


    void WriteData(const QByteArray data);
    void ReadData();
    void on_pushButtonConnect_clicked();

    void on_pushButtonDisconnect_clicked();
    void updateTemperature(QString sensor_reading);
    void updateHumidity(QString sersor_reading);
    void readSerial();
  //  void sendData(QString data);
    void on_pushButtonTurnOn_clicked();

    void on_pushButtonTurnOff_clicked();


    void on_pushButtonTurnOffSpeaker_clicked();

    void on_pushButtonStartHatch_clicked();

    void on_pushButtonStopHatch_clicked();

    void on_pushButtonContinue_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *devserial;
    comserial *procSerial;
    QByteArray serialData;
        QString serialBuffer;
        QString parsed_data;
        QString temperature_value;
        QString humidity_value;




      void CarregarInfoDispSerial(void);
};
#endif // MAINWINDOW_H
