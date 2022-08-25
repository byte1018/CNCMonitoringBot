#ifndef MQTTCONNECTIONDISPATCHER_H
#define MQTTCONNECTIONDISPATCHER_H

#include <QObject>
#include <qmqtt.h>
#include "mqttclient.h"
#include <qmqtt_message.h>
#include <configxmllib.h>

struct tMqttConnection
{
    QString                 Name;
    MqttClient              *MqttClientPtr;
    int                     Index;
};

typedef QMap<QString,tMqttConnection> tMqttConnections;


class MqttConnectionDispatcher : public QObject
{
    Q_OBJECT
public:

    explicit MqttConnectionDispatcher(QObject *parent = nullptr);

    bool Init(mqttMsgHandler aMsgHandler);
    void Send(QByteArray aMsg, QString aTopic = "",bool aUseAllServers = false, QString aConnectionName = "");
    void onChangeStateConnection(QString aNameConnection);
private:

    ConfigXmlLib Config;

    QString DefaultConnectionName       = "";
    int     DefaultQos                  = 0 ;
    bool    DefaultRetrain              = false;
    int     DefaultReconnectInterval    = 1000 ;

    QString strConfigName               = "MQTT";
    QString strPlace                    = "server";
    QString strHost                     = "host";
    QString strPort                     = "port";
    QString strIndex                    = "flat_index";
    QString strName                     = "name";

    QString strDefaultHost              = "localhost";
    QString strDefaultPort              = "1883";
    QString strMqttPrifix               = "mqtt://";
    QString strClientID                 = "client_id";
    QString strDefaultClientID          = "cliendID";

    QString strType                     = "type";
    QString strOut                      = "out";
    QString strLabel                    = "label";
    QString strSub                      = "subj";

    QString strUser                     = "user";
    QString strPwd                      = "pwd";
    QString strQos                      = "qos";
    QString strRetrain                  = "retrain";
    QString StrReconnectInterval        = "reconnect";

};

#endif // MQTTCONNECTIONDISPATCHER_H
