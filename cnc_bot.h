#ifndef CNC_BOT_H
#define CNC_BOT_H

#include "QObject"
#include "QTimer"
#include "QMap"
#include <QThread>


#include "tgbot/tgbot.h"
#include "tgbot/net/CurlHttpClient.h"
#include "tgbot/EventHandler.h"

#include <mqttconnectiondispatcher.h>
#include <pollthread.h>
#include <datasource.h>
#include <configxmllib.h>

#include "QString"
#include <QStringList>
#include <QDateTime>
#include "QJsonDocument"
#include "QJsonObject"

using namespace std;
using namespace TgBot;

enum StateDevice
{
    sd_connected           = 0,
    sd_disconnected,
    sd_restart,
    sd_flashing,
    sd_pause
};
enum EventDevice
{
    ev_connected           = 0,
    ev_disconnected,
    ev_restart,
    ev_flashing,
    ev_stop,
    ev_start
};

enum ConfigIndexes
{
    cfg_max_current         = 0,
    cfg_delta,
    cfg_version,
    cfg_state,
    cfg_description,
    сfg_mqtt_id,
    cfg_mac,
    cfg_isregistred,
    cfg_work_place,
    cfg_free_ram,
    cfg_current,
    cfg_currentTS,
    cfg_mesh_layer

};

enum TypeMsgIndexes
{
    tp_msg_info             = 0,
    tp_msg_diag,
    tp_msg_data,
    tp_msg_cmd

};

enum CmdIndexes
{
    cmd_get_config          = 0,
    cmd_set_config,
    cmd_flash,
    cmd_stop_mon,
    cmd_start_mon,
    cmd_restart_esp
};




struct tDevice
{
    float       MaxCurrent {0};
    int         Delta{0};
    QString     FWVersion{""};
    int         State {sd_disconnected};
    QString     EquipmentName{""};
    QString     WorkplaceName{""};
    QString     MqttClientID{""};
    bool        isRegistred{false};
    qint64      LastInfoTS;
    int         FreeRam{0};
    float       LastCurrent;
    qint64      LastCurrentTS;
    int         MeshLayer{-1};


};
struct tChat
{
    QString         curDevice{""};
    Message::Ptr    LastCmd {nullptr};
    Message::Ptr    LastMsg {nullptr};
    qint64          LastSendTS{0};
};

typedef QMap<quint64, tChat> tListChats;
typedef QMap<QString,tDevice> tListDevices ;

class cnc_bot : public QObject
{

public:

    cnc_bot(string _token): tgbot{_token,_curlHttpClient}
    {

    };

    void Init();
    void onMsg(const Message::Ptr message);
    void onCmd(const Message::Ptr message);
    void onCallBackQuery(const CallbackQuery::Ptr callbackQuery);
    void onInlineQuery(const InlineQuery::Ptr inlineQuery);
    void onMqttMessage (QString aNameConnection,const QMQTT::Message& aMessage);

    QString Get_All_Info_str             = "{\"TypeMsg\":\"CMD\",\"mac_address\":\"ALL\",\"CMD\":\"CMD_GET_CONFIG\"}";

    QMap<int,QString> ListStatesDevice
    {
        {sd_connected,"Подключено"},
        {sd_disconnected,"Отключено"},
        {sd_pause,"Остановлено"},
        {sd_restart,"Перезапуск"},
        {sd_flashing,"Прошивка"}
    };

    QMap<int,QString> ListCfgLabel
    {
        {cfg_max_current,   "Максимальный ток (А)"},
        {cfg_delta,         "Порог изменения тока (%)"},
        {cfg_version,       "Версия прошивки"},
        {cfg_state,         "Состояние"},
        {cfg_description,   "Описание"},
        {сfg_mqtt_id,       "Имя клиета Mqtt"},
        {cfg_mac,           "МАС адрес"},
        {cfg_isregistred,   "Зарегистрирован"},
        {cfg_work_place,    "Расположение"},
        {cfg_free_ram,      "Свободное ОЗУ"},
        {cfg_current,       "Последнее значение тока"},
        {cfg_currentTS,     "Отправлено в"},
        {cfg_mesh_layer,    "Уровень mesh"}
    };


    QString current_str                  = "current";

    QMap<int,QString> ListCfg
    {
        {cfg_max_current,   "max_current"},
        {cfg_delta,         "delta_current"},
        {cfg_version,       "version"},
        {сfg_mqtt_id,       "mqtt_clientid"},
        {cfg_mac,           "mac_address"},
        {cfg_free_ram,      "free_memory"},
        {cfg_current,       "current"},
        {cfg_currentTS,     "timestamp"},
        {cfg_mesh_layer,    "mesh_layer"}



    };

    QMap<int,QString> ListEventDevice
    {
        {ev_connected,"connected"},
        {ev_disconnected,"disconnected"},
        {ev_restart,"restart"},
        {ev_flashing,"flashing"},
        {ev_stop,"stop"},
        {ev_start,"start"}
    };


    QMap<QString,int> ListCodeEventDevice
    {
        {ListEventDevice[ev_connected] ,ev_connected},
        {ListEventDevice[ev_disconnected],ev_disconnected},
        {ListEventDevice[ev_restart],ev_restart},
        {ListEventDevice[ev_flashing],ev_flashing},
        {ListEventDevice[ev_stop],ev_stop},
        {ListEventDevice[ev_start],ev_start}
    };

    QMap<int,QString> ListTypeMsg
    {
        {tp_msg_info,"INFO"},
        {tp_msg_diag,"DIAG"},
        {tp_msg_data,"DATA"},
        {tp_msg_cmd,"CMD"}

    };

    QMap<int,QString> ListCmd
    {

        {cmd_get_config,"CMD_GET_CONFIG"},
        {cmd_set_config,"CMD_SET_CONFIG"},
        {cmd_flash,"CMD_FLASH"},
        {cmd_stop_mon,"CMD_STOP_MON"},
        {cmd_start_mon,"CMD_START_MON"},
        {cmd_restart_esp,"CMD_RESTART_ESP"}

    };

    QMap<int,QString> ListCmdLabel
    {
        {cmd_get_config,"Получить конфигурацию"},
        {cmd_set_config,"Установить конфигурацию"},
        {cmd_flash,"Записать ПО"},
        {cmd_stop_mon,"Остановить мониторинг"},
        {cmd_start_mon,"Запустить мониторинг"},
        {cmd_restart_esp,"Перезапустить устройство"}

    };

    QString CMD_str                     = "CMD";
    QString Type_Msg_str                = "TypeMsg";
    QString Event_str                   = "EVENT";

    QString Query_device_str            = "Device";
    QString Query_main_menu_str         = "MainMenu";

    QString ButtonText_main_menu_str    = "Cписок устройств";
    QString ButtonText_refresh_str      = "Обновить";
    QString ButtonText_SendContact_str  = "Отправить контакт";



    QString OTA_URL_str                 = "OTA_URL";
    QString OTA_URL_tag_str             = "ota_url";

    QString TS_str                      = "timestamp";
    QString All_str                     = "ALL";

    QString ConfigName_str              =   "BOT";
    QString Place_str                   =   "SET";


    QString OTA_URL {""};
    QString ret{"\n"};
    QString delim{" - "};
    QString query_delim {"*"};

    void onPingTimer();

private:

    Message::Ptr SendMainMenu(uint64_t aChatid, int aMsgId = 0, QString aText="");
    void SendDevMenu(uint64_t aChatid, QString aTypeMsg="");

    QString fromUtf8String(std::string aString);

    QString GetDeviceText(QString aDevice);
    InlineKeyboardMarkup::Ptr GetMainMenu();
    InlineKeyboardMarkup::Ptr GetDeviceKbd(QString aMac);

    ReplyKeyboardMarkup::Ptr GetRequestPhoneKbd();
    ReplyKeyboardMarkup::Ptr GetRequestLocationKbd();



    void BuildDevicesList();
    void CheckDevice(QByteArray aMsg);
    void AddDevice(QString aMac, int aState = sd_disconnected);

    void ReadConfig();


    ConfigXmlLib Config;


    Bot tgbot;
    PollThread poll_thread {tgbot};
    CurlHttpClient _curlHttpClient;

    //quint64 ChatID {18446744073066346070};
    //quint64 ChatID {1675141800};

    MqttConnectionDispatcher mqtt_disp;
    DataSource DS;
    tListChats ListChats;
    tListDevices ListDevices;
    QTimer PingDeviceTimer;

    int SendMsgIntervalMin{1200};
    int PingDeviceInteval{SendMsgIntervalMin*8};
    int TimeOutDevice{static_cast<int>(PingDeviceInteval*1.2)};

};

#endif // CNC_BOT_H
