#ifndef RECEIVER_H
#define RECEIVER_H


#include <QObject>
#include <QHostAddress>
#include <QByteArray>
#include <QTimer>


class QUdpSocket;



class UdpReceiver : public QObject
{
Q_OBJECT
public:
    UdpReceiver(QObject *parent=nullptr);


signals:
    void dataReceived(const QByteArray &data);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    QTimer *udpTimer;
};

#endif
