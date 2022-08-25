#include "mqttclient.h"

MqttClient::MqttClient()
{
    connect(this, &Client::connected, this, &MqttClient::onConnect);
    connect(this, &Client::received, this, &MqttClient::onReceive);
}

void MqttClient::onConnect()
{
    printf("%s connected \n",Name.toStdString().c_str());

    auto itSub = InSub.cbegin();
    while (itSub != InSub.cend())
    {
        subscribe(itSub.key(),Qos);
        itSub.operator++();
    }

    while ((QueueMessages.count()>0) && (connectionState() == QMQTT::STATE_CONNECTED))
    {
        auto Msg = QueueMessages.dequeue();
        Send(Msg.PayLoad,Msg.Topic);
    }


}
void MqttClient::onReceive(const QMQTT::Message& message)
{
    MsgHandler(Name,message);
}

void MqttClient::Send(QByteArray aMsg, QString aTopic)
{
    QMQTT::Message message;
    if (connectionState() == QMQTT::STATE_CONNECTED)
    {
        if (aTopic == "") {aTopic=DefaultOutSub;}

        if (aTopic != "")
        {
            message.setPayload(aMsg);
            message.setQos(Qos);
            message.setRetain(Retrain);
            message.setTopic(aTopic);
            publish(message);
        }
    }
    else
    {
        tAttribQueueMessage msg;
        msg.PayLoad = aMsg;
        msg.Topic = aTopic;
        QueueMessages.enqueue(msg);
    }
}
