#include "serial_com.h"
#include <QSerialPort>
#include <QTimer>


SerialBack::SerialBack(QObject *parent) : ICommunication(parent) {}


SerialBack::~SerialBack() {}


bool SerialBack::open(const QString &arg1, qint32 arg2) {
    serial_port = new QSerialPort(arg1, this);
    timer = new QTimer();

    serial_port->setBaudRate(arg2);
    serial_port->setDataBits(QSerialPort::Data8);
    serial_port->setParity(QSerialPort::NoParity);
    serial_port->setStopBits(QSerialPort::OneStop);
    serial_port->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial_port->open(QIODevice::ReadWrite)) {
        delete serial_port;
        return false;
    }

    connect(serial_port, &QSerialPort::readyRead, this,
            &SerialBack::handle_ready_read);
    connect(timer, &QTimer::timeout, this, &SerialBack::handle_timer);
    timer->start(100);
    return true;
}


void SerialBack::handle_ready_read(void) {
    QByteArray *data = new QByteArray(serial_port->readAll());

    if (!data->isEmpty()) {
        emit data_is_ready(*data);
    }
}


void SerialBack::handle_timer(void) {
    timer->start(50);
    serial_port->waitForReadyRead(10);
}


void SerialBack::write(const QByteArray &data) {
    serial_port->write(data);
    serial_port->waitForBytesWritten(10);
}


void SerialBack::close(void) {
    if (serial_port->isOpen()) {
        timer->stop();
        serial_port->close();
        disconnect(serial_port, &QSerialPort::readyRead, this,
                   &SerialBack::handle_ready_read);
        disconnect(timer, &QTimer::timeout, this, &SerialBack::handle_timer);

        delete timer;
        delete serial_port;
    }
}
