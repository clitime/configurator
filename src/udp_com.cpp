#include "udp_com.h"


#include "serial_com.h"
#include <QNetworkDatagram>
#include <QRegExp>
#include <QTimer>
#include <QUdpSocket>


UdpBack::UdpBack(QObject *parent) : ICommunication(parent) {
    local_port = 7755;
}


UdpBack::~UdpBack() {}


bool UdpBack::open(const QString &arg1, qint32 arg2) {
    QRegExp re("(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])(\\.(25[0-5]|"
               "2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])){3}");
    if (!re.exactMatch(arg1)) {
        return false;
    }

    host_ip = QHostAddress(arg1);
    host_port = static_cast<quint16>(arg2);

    udpSocket = new QUdpSocket();
    if (!udpSocket->bind(QHostAddress::AnyIPv4, local_port)) {
        return false;
    }

    connect(udpSocket, &QUdpSocket::readyRead, this,
            &UdpBack::handle_ready_read);
    return true;
}


void UdpBack::handle_ready_read(void) {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();

        if (datagram.isValid()) {
            emit data_is_ready(datagram.data());
        }
    }
}


void UdpBack::write(const QByteArray &data) {
    udpSocket->writeDatagram(data, host_ip, host_port);
}


void UdpBack::close() {
    disconnect(udpSocket, &QUdpSocket::readyRead, this,
               &UdpBack::handle_ready_read);
    udpSocket->disconnectFromHost();
    udpSocket->close();
    delete udpSocket;
}
