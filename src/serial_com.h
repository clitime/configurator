#pragma once

#include "icommunication.h"

class QByteArray;
class QSerialPort;
class QTimer;

class SerialBack : public ICommunication {
    Q_OBJECT
private:
    QSerialPort *serial_port = nullptr;
    QTimer *timer = nullptr;

public:
    explicit SerialBack(QObject *parent = nullptr);
    ~SerialBack();
    void write(const QByteArray &data);
public slots:
    bool open(const QString &arg1, qint32 arg2);
    void close(void);
private slots:
    void handle_ready_read(void);
    void handle_timer(void);
};
