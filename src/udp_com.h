#pragma once


#include "icommunication.h"
#include <QHostAddress>


class QByteArray;
class QUdpSocket;
class QHostAddress;


class UdpBack : public ICommunication {
    Q_OBJECT
private:
    QUdpSocket *udpSocket = nullptr;
    quint16 local_port;
    QHostAddress host_ip;
    quint16 host_port;

public:
    explicit UdpBack(QObject *parent = nullptr);
    ~UdpBack();
    void write(const QByteArray &data);
public slots:
    bool open(const QString &arg1, qint32 arg2);
    void close(void);
private slots:
    void handle_ready_read(void);
};
