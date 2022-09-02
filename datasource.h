#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <configxmllib.h>
#include "QtSql/QSqlDatabase"
#include "QtSql/QSqlQuery"
#include "QtSql/QSqlRecord"
#include "QtSql/QSqlError"

class DataSource
{
public:
    DataSource();
    void Init();
    bool CheckUserID(quint64 aUserID);
    bool CheckUserName(QString aUserName);
    bool CheckUserPhone(qint64 aUserID, QString aUserName, QString aFirstName, QString aPhone);

    QString GetDeviceField(QString aMac,QString aFieldName);
    QString GetDeviceEquipmentName  (QString aMac);
    QString GetDeviceWorkplaceName  (QString aMac);
    QString GetDeviceMqttClientID   (QString aMac);

private:
    ConfigXmlLib Config;

    QSqlDatabase Db;
    QSqlQuery tbUsers, tbDevices;


    QString strConfigName               = "DATABASE";
    QString strPlace                    = "server";

    QString strIsActive                 = "active";
    QString strTypeDb                   = "QMYSQL";
    QString strDb                       = "mqtt";

    QString strHost                     = "host";
    QString strPort                     = "port";
    QString strUser                     = "user";
    QString strPwd                      = "pwd";

    QString strDefaultHost              = "localhost";
    QString strDefaultPort              = "3306";

    QString strDeviceSelectBase         =   "SELECT MAC_Address, MqttClientID, EquipmentName, WorkplaceName "
                                            "FROM EquipmentSettings ES "
                                            "JOIN Equipments EQ on EQ.EquipmentSettingsID = ES.ID "
                                            "JOIN Workplaces WP ON WP.ID = EQ.WorkPlaceID "
                                            "Where MAC_Address ";
    QString strDeviceSelectAll          =    strDeviceSelectBase+" IS NOT NULL; ";

    QString strMqttClientID             =   "MqttClientID";
    QString strEquipmentName            =   "EquipmentName";
    QString strWorkplaceName            =   "WorkplaceName";
    //QString str

};

#endif // DATASOURCE_H
