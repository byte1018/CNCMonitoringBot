#include "mqttconnectiondispatcher.h"

static tMqttConnections MqttConnections;

MqttConnectionDispatcher::MqttConnectionDispatcher(QObject *parent) : QObject(parent)
{

}

bool MqttConnectionDispatcher::Init(mqttMsgHandler aMsgHandler)
{
    //qInstallMessageHandler(&(LogError.Dispatch));
    bool result = false;

    if ( (Config.isParsedOK()) &  Config.SelectConfig(strConfigName)  )
    {
        tAttributes ServerAttrib = Config.GetPlaceFirst(strPlace);
        bool EoServers = (ServerAttrib.count() == 0);
        tMqttConnection mqtt_connection;
        tObjects subs;


        // Ищем в конфиге описания соединений mqtt
        while (!EoServers)
        {
            // Заполняем поля параметров соединения mqtt
            QString sIndex = ServerAttrib.value(strIndex,1).toString();
            mqtt_connection.Index = ServerAttrib.value(strIndex,1).toInt();
            mqtt_connection.Name =ServerAttrib.value(strName,"").toString();

            mqtt_connection.MqttClientPtr = new MqttClient();
            mqtt_connection.MqttClientPtr->Qos = ServerAttrib.value(strQos,DefaultQos).toInt();
            mqtt_connection.MqttClientPtr->Retrain = ServerAttrib.value(strRetrain,DefaultRetrain).toBool();
            mqtt_connection.MqttClientPtr->Name =  mqtt_connection.Name;
            mqtt_connection.MqttClientPtr->Index = mqtt_connection.Index;

            mqtt_connection.MqttClientPtr->MsgHandler = aMsgHandler;

            mqtt_connection.MqttClientPtr->setHost(QHostAddress(ServerAttrib.value(strHost,strDefaultHost).toString()));
            mqtt_connection.MqttClientPtr->setPort(ServerAttrib.value(strPort,strDefaultPort).toInt());
            mqtt_connection.MqttClientPtr->setClientId(ServerAttrib.value(strClientID,strDefaultClientID+sIndex).toString());
            mqtt_connection.MqttClientPtr->setUsername(ServerAttrib.value(strUser,"").toString());
            mqtt_connection.MqttClientPtr->setPassword(ServerAttrib.value(strPwd,"").toByteArray());
            mqtt_connection.MqttClientPtr->setAutoReconnectInterval(ServerAttrib.value(StrReconnectInterval,DefaultReconnectInterval).toInt());

            mqtt_connection.MqttClientPtr->InSub.clear();
            mqtt_connection.MqttClientPtr->OutSub.clear();


            if (DefaultConnectionName =="") {DefaultConnectionName = mqtt_connection.Name;}
            subs = Config.GetObjects();
            // перебираем группы и раскидываем по спискам.
            auto itSub = subs.cbegin();
            while (itSub != subs.cend())
            {
                auto out = itSub.value().Atributes.value(strType,"").toString() ;
                auto Subj = itSub.value().Atributes.value(strSub,"").toString() ;

                if (out == strOut)
                {
                    mqtt_connection.MqttClientPtr->OutSub.insert(Subj, itSub.value().Atributes) ;
                    if (mqtt_connection.MqttClientPtr->DefaultOutSub == "") {mqtt_connection.MqttClientPtr->DefaultOutSub = Subj;}
                }
                else
                {
                    mqtt_connection.MqttClientPtr->InSub.insert(Subj,itSub.value().Atributes);
                }
                itSub.operator++();
            }

            MqttConnections.insert(mqtt_connection.Name, mqtt_connection);

            mqtt_connection.MqttClientPtr->connectToHost();

            ServerAttrib = Config.GetPlaceNext();
            EoServers = (ServerAttrib.count() == 0);
        }
        result = true;
    }
    return result;
}
void MqttConnectionDispatcher::Send(QByteArray aMsg, QString aTopic, bool aUseAllServers, QString aConnectionName)
{
    if (aUseAllServers)
    {
        for (auto Conn : MqttConnections)
        {
            Conn.MqttClientPtr->Send(aMsg, aTopic);
        }
    }
    else
    {
        QString ConnectionName = aConnectionName;
        if (ConnectionName =="")
            {ConnectionName = DefaultConnectionName;}
        MqttConnections[ConnectionName].MqttClientPtr->Send(aMsg,aTopic);
    }
}
