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
    cfg_work_place

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
    float   MaxCurrent {0};
    int     Delta{0};
    QString FWVersion{""};
    int     State {sd_disconnected};
    QString EquipmentName{""};
    QString WorkplaceName{""};
    QString MqttClientID{""};
    bool    isRegistred{false};

};
struct tChat
{
    QString curDevice{""};
    Message::Ptr LastCmd {nullptr};
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
    InlineKeyboardMarkup::Ptr GetDeviceKbd(QString aMac);
    QString GetDeviceText(QString aDevice);
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



    QString Get_All_Info_str             = "{\"+TypeMsg\":\"CMD\",\"mac_address\":\"ALL\",\"CMD\":\"CMD_GET_CONFIG\"}";

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
        {cfg_max_current,   "Максимальный ток (А) "},
        {cfg_delta,         "Порог изменения тока (%) "},
        {cfg_version,       "Версия прошивки "},
        {cfg_state,         "Состояние "},
        {cfg_description,   "Описание "},
        {сfg_mqtt_id,       "Имя клиета Mqtt "},
        {cfg_mac,           "МАС адрес "},
        {cfg_isregistred,   "Зарегистрирован "},
        {cfg_work_place,    "Расположение "}

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
    QString SendContact_str             = "Отправить контакт";


private:


    void CheckDevice(QByteArray aMsg);
    Message::Ptr SendMainMenu(uint64_t aChatid, int aMsgId = 0, QString aText="");
    QString fromUtf8String(std::string aString);

    ReplyKeyboardMarkup::Ptr GetRequestPhoneKbd();
    ReplyKeyboardMarkup::Ptr GetRequestLocationKbd();


    Bot tgbot;
    PollThread poll_thread {tgbot};
    CurlHttpClient _curlHttpClient;

    //quint64 ChatID {18446744073066346070};
    //quint64 ChatID {1675141800};

    MqttConnectionDispatcher mqtt_disp;
    DataSource DS;
    tListChats ListChats;
    tListDevices ListDevices;

};

#endif // CNC_BOT_H
