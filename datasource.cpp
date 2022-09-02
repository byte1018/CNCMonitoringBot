#include "datasource.h"
#include "QtWidgets/qapplication.h"

DataSource::DataSource()
{

}
bool DataSource::CheckUserID(quint64 aUserID)
{
    bool result = false;

    tbUsers.exec("select TelegrammUserID from BotUsers where TelegrammUserID = "+ QString::number(aUserID));
    if (tbUsers.size() >0)
    {
        result = true;
    }
    return result;
}

bool  DataSource::CheckUserName(QString aUserName)
{
    bool result = false;

    tbUsers.exec("select UserName from BotUsers where TelegrammUserID = "+ aUserName);
    if (tbUsers.size() >0)
    {
        result = true;
    }
    return result;
}

bool DataSource::CheckUserPhone(qint64 aUserID, QString aUserName, QString aFirstName, QString aPhone)
{
    bool result = false;
    QString Phone = aPhone.right(10);
    tbUsers.exec("select Phone from BotUsers where Phone = "+ Phone);
    if (tbUsers.size() >0)
    {
        result = true;
        QString SqlText = "update BotUsers SET Name = '"+aFirstName+"'";
        SqlText += " , UserName = '"+aUserName+"'";
        SqlText += " ,TelegrammUserID = "+ QString::number(aUserID);
        SqlText += " where Phone = '"+ Phone+"'";
        tbUsers.exec(SqlText);
        QSqlError err = tbUsers.lastError();
        if (err.type() != QSqlError::NoError)
        {
            printf("Update error - %s\n%s \n",err.text().toStdString().c_str(),tbUsers.lastQuery().toStdString().c_str());
        }
    }
    return result;
}
void DataSource::Init()
{
    if ( (Config.isParsedOK()) &  Config.SelectConfig(strConfigName)  )
    {
        tAttributes ServerAttrib = Config.GetPlaceFirst(strPlace);
        bool EoServers = (ServerAttrib.count() == 0);

        while (!EoServers)
        {
            bool isActive = ServerAttrib.value(strIsActive,true).toBool();

            if (isActive)
            {
                Db = QSqlDatabase::addDatabase(strTypeDb,strDb);
                Db.setDatabaseName(strDb);
                Db.setHostName(ServerAttrib.value(strHost,strDefaultHost).toString());
                Db.setPort(ServerAttrib.value(strPort,strDefaultPort).toInt());
                Db.setUserName(ServerAttrib.value(strUser,"").toString());
                Db.setPassword(ServerAttrib.value(strPwd,"").toString());
            }
            ServerAttrib = Config.GetPlaceNext();
            EoServers = (ServerAttrib.count() == 0);
        }

        QSqlError err ;
        Db.open();
        if (!Db.isOpen())
        {
            err = Db.lastError();
            printf("Open base error -  %s\n",err.text().toStdString().c_str());
        }

        tbUsers = QSqlQuery(Db);
        tbDevices = QSqlQuery(Db);



    }
}

QString DataSource::GetDeviceEquipmentName(QString aMac)
{
    return GetDeviceField(aMac,strEquipmentName);
}
QString DataSource::GetDeviceWorkplaceName(QString aMac)
{
    return GetDeviceField(aMac,strWorkplaceName);
}
QString DataSource::GetDeviceMqttClientID(QString aMac)
{
    return GetDeviceField(aMac,strMqttClientID);
}

QString DataSource::GetDeviceField(QString aMac, QString aFieldName)
{
    QString result ="";
    tbDevices.exec(strDeviceSelectBase+" = '"+aMac+"'");
    if (tbDevices.size() >0)
    {
        tbDevices.first();
        int fieldNo = tbDevices.record().indexOf(aFieldName);
        result = tbDevices.value(fieldNo).toString();
    }

    return result;

}


















