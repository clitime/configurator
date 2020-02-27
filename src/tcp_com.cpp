#include "tcp_com.h"


#include "serial_com.h"
#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>


TcpBack::TcpBack(QObject *parent) : ICommunication(parent) {
    local_port = 7755;
}


TcpBack::~TcpBack() {}


bool TcpBack::open(const QString &arg1, qint32 arg2) {
    if (arg1 == "") {
        return false;
    }

    host_ip = QHostAddress(arg1);
    host_port = static_cast<quint16>(arg2);

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, &QTcpSocket::readyRead, this,
            &TcpBack::handle_ready_read);
    timer = new QTimer();
    timer->start(100);

    return true;
}


void TcpBack::handle_ready_read(void) {
    QString buff;
    qint32 msgSize = -1;

    QDataStream stream(tcpSocket);
    while (true) {
        if (msgSize < 0) {
            if (tcpSocket->bytesAvailable() <
                static_cast<qint64>(sizeof(int))) {
                break;
            }
            stream >> msgSize;
        } else {
            if (tcpSocket->bytesAvailable() < msgSize) {
                break;
            }
            stream >> buff;
            QByteArray *data = new QByteArray(buff.toStdString().c_str());
            emit data_is_ready(*data);
            msgSize = -1;
        }
    }
    tcpSocket->disconnectFromHost();
}


void TcpBack::write(const QByteArray &data) {
    tcpSocket->connectToHost(host_ip, host_port);
    tcpSocket->write(data);
}


void TcpBack::close() {
    timer->stop();
    delete timer;
    disconnect(tcpSocket, &QTcpSocket::readyRead, this,
               &TcpBack::handle_ready_read);
    tcpSocket->close();
    delete tcpSocket;
}
