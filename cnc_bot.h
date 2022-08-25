#ifndef CNC_BOT_H
#define CNC_BOT_H

#include "QObject"
#include "QTimer"
#include "QMap"

#include "tgbot/tgbot.h"
#include "tgbot/net/CurlHttpClient.h"
#include "tgbot/EventHandler.h"

#include <mqttconnectiondispatcher.h>
#include "resource.h"

#include "QString"
#include <QStringList>
#include <QDateTime>
#include "QJsonDocument"
#include "QJsonObject"

using namespace std;
using namespace TgBot;

struct tDevice
{
    float   MaxCurrent {0};
    int     Delta{0};
    QString FWVersion{""};
    int     State;

};
struct tChat
{
    QString curDevice{""};
    bool isInited{false};
    Message::Ptr LastMsg {nullptr};
};

typedef QMap<quint64, tChat> tListChats;
typedef QMap<QString,tDevice> tListDevices ;

class cnc_bot : public QObject
{
public:

    cnc_bot(string _token): tgbot{_token,_curlHttpClient}
    {

    };
    void onMsg(const Message::Ptr message);
    void onCmd(const Message::Ptr message);
    void onCallBackQuery(const CallbackQuery::Ptr callbackQuery);
    void onInlineQuery(const InlineQuery::Ptr inlineQuery);
    void onMqttMessage (QString aNameConnection,const QMQTT::Message& aMessage);
    InlineKeyboardMarkup::Ptr GetMainMenu();
    void Init();


    QString mac_adress_str               = "mac_address";
    QString current_str                  = "current";

    QString config_max_current_str       = "max_current";
    QString config_delta_current_str     = "delta_current";
    QString config_version_str           = "version";


    QString OTA_URL_str                  = "OTA_URL";
    QString TS_str                       = "timestamp";
    QString Current_str                  = "current";
    QString All_str                      = "ALL";

    QString Type_Msg_str                 = "TypeMsg";
    QString Type_Msg_Info_str            = "INFO";
    QString Type_Msg_Diag_str            = "DIAG";
    QString Type_Msg_Data_str            = "DATA";
    QString Type_Msg_CMD_str             = "CMD";

    QString Type_Msg_event_str           = "EVENT";
    QString Event_connected_str          = "connected";
    QString Event_flashing_str           = "flashing";
    QString Event_disconnected_str       = "disconnected";
    QString Event_restart_str            = "restart";

    QString CMD_str                      = "CMD";
    QString CMD_GET_CONFIG_str           = "CMD_GET_CONFIG";
    QString CMD_SET_CONFIG_str           = "CMD_SET_CONFIG";
    QString CMD_FLASH_str                = "CMD_FLASH";
    QString CMD_STOP_MON_str             = "CMD_STOP_MON";
    QString CMD_START_MON_str            = "CMD_START_MON";
    QString CMD_RESTART_ESP_str          = "CMD_RESTART_ESP";
private:

    void StartPoll();
    void CheckDevice(QByteArray aMsg);
    ReplyKeyboardMarkup::Ptr Keyboard;

    Bot tgbot;
    QTimer BotUpdateTimer;
    const int BotUpdateInterval{10};
    TgLongPoll longPoll{tgbot,100,BotUpdateInterval};
    CurlHttpClient _curlHttpClient;

    //quint64 ChatID {18446744073066346070};
    //quint64 ChatID {1675141800};

    MqttConnectionDispatcher mqtt_disp;
    tListChats ListChats;
    tListDevices ListDevices;

};

#endif // CNC_BOT_H
