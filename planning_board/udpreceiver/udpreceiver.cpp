#include <QUdpSocket>
#include <QDebug>
#include <QString>
#include "udpreceiver.h"


UdpReceiver::UdpReceiver(QObject *parent) :
    QObject(parent),
    udpSocket(new QUdpSocket(this)),
    udpTimer(new QTimer)

{
    QObject::connect(udpTimer,&QTimer::timeout,[=](){
        if (udpSocket->bind(UDP_PORT, QUdpSocket::ShareAddress))
            udpTimer->stop();
    });
    QObject::connect(udpSocket,&QUdpSocket::disconnected,[=](){udpTimer->start(5000);});
    QObject::connect(udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::processPendingDatagrams);
    udpTimer->start(5000);
}




void UdpReceiver::processPendingDatagrams()
{
    if (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        QHostAddress udpPlcHost;
        udpSocket->readDatagram(datagram.data(), datagram.size(), &udpPlcHost);
        if(udpPlcHost.toString().contains(UDP_PLC_HOST))
            dataReceived(datagram);
    }
}


