#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include "qmqtt.h"
#include <configxmllib.h>

typedef  void (*mqttMsgHandler) (QString aNameConnection, const QMQTT::Message& message);

struct tAttribQueueMessage
{
    QByteArray              PayLoad;
    QString                 Topic;
};

typedef QQueue<tAttribQueueMessage> tQueueMessages;

class MqttClient : public QMQTT::Client
{
    Q_OBJECT
public:
    MqttClient();
    QString             Name;
    int                 Index;
    int                 Qos{0};
    bool                Retrain{false};
    tElements           InSub;
    tElements           OutSub;
    QString             DefaultOutSub ="";

    mqttMsgHandler      MsgHandler;

    void                Send(QByteArray aMsg, QString aTopic = "");
private:
    quint64             QueueKey {0};
    tQueueMessages      QueueMessages;
public slots:
    void                onConnect();
    void                onReceive(const QMQTT::Message &message);


};

#endif // MQTTCLIENT_H
