#pragma once


#include "icommunication.h"
#include <QHostAddress>


class QByteArray;
class QTcpSocket;
class QHostAddress;
class QTimer;


class TcpBack : public ICommunication {
    Q_OBJECT
private:
    QTcpSocket *tcpSocket = nullptr;
    QTimer *timer = nullptr;
    quint16 local_port;
    QHostAddress host_ip;
    quint16 host_port;

public:
    explicit TcpBack(QObject *parent = nullptr);
    ~TcpBack();
    void write(const QByteArray &data);
public slots:
    bool open(const QString &arg1, qint32 arg2);
    void close(void);
private slots:
    void handle_ready_read(void);
};
