#include "cnc_bot.h"


using namespace TgBot;

cnc_bot *ChatClassPtr;

void Dispatcher_cmd(const Message::Ptr message)
{
    ChatClassPtr->onCmd(message);
}
void Dispatcher_msg(const Message::Ptr message)
{
    ChatClassPtr->onMsg(message);
}
void Dispatcher_callbackquery(const CallbackQuery::Ptr callbackQuery)
{
    ChatClassPtr->onCallBackQuery(callbackQuery);
}
void Dispatcher_inlineQuery(const InlineQuery::Ptr inlineQuery)
{
    ChatClassPtr->onInlineQuery(inlineQuery);
}

void Dispatcher_onMqttMessage (QString aNameConnection,const QMQTT::Message& aMessage)
{
    ChatClassPtr->onMqttMessage(aNameConnection,aMessage);
}
/**
 * @brief onMsg
 * @param message
 */
void cnc_bot::onMsg(const Message::Ptr message)
{
    Q_UNUSED(message);
    if (message->contact != nullptr)
    {
        auto contact = message->contact;
        printf("%ld %s %s %s \n",contact->userId,contact->phoneNumber.c_str(), contact->firstName.c_str(), message->from->username.c_str());

        bool result = DS.CheckUserPhone(contact->userId,
                                        fromUtf8String(message->from->username),
                                        fromUtf8String(contact->firstName),
                                        fromUtf8String(contact->phoneNumber)
                                        );
        if (result)
        {
            tgbot.getApi().sendMessage(message->chat->id,"Идентификация по номеру телефона была успешной. ",false, 0);
        }
        else
        {
            string text =  "Идентификация по номеру телефона неудачна. Обратитесь к администратору для предоставления соответствующих прав";
            tgbot.getApi().sendMessage(message->chat->id,text,false, 0);
        }

    }
}
Message::Ptr cnc_bot::SendMainMenu(uint64_t aChatid, int aMsgId, QString aText)
{
    Message::Ptr result;
    if (ListDevices.count() > 0)
    {
        aText += "Выберите устройство из списка. ";
    }
    else
    {
        aText += "Устройства не найдены. ";
    }
    aText += "("+QTime::currentTime().toString()+")";
    if (aMsgId == 0)
    {
        result = tgbot.getApi().sendMessage(aChatid,aText.toStdString(),false, 0, GetMainMenu());
    }
    else
    {
        result = tgbot.getApi().editMessageText(aText.toStdString(),aChatid, aMsgId,"","",false,GetMainMenu());
    }
    return result;
}
QString cnc_bot::fromUtf8String(std::string aString)
{
    QString result="";
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
     if (codec)
     {

       QByteArray ba(aString.c_str());                  // Convert to QByteArray
       result = codec->toUnicode(ba);                   // Qt magic !!!
     }
    return result;
}
void cnc_bot::onCmd(const Message::Ptr message)
{
    uint64_t chatid = message->chat->id;
    tChat chat;
    bool isAutorized = false;

    if (ListChats.find(chatid) == ListChats.constEnd())
    {
        ListChats.insert(chatid,chat);
    }


    if (message->chat->type == Chat::Type::Private)
    {
        if (DS.CheckUserID(message->from->id))
        {
            isAutorized = true;
        }
        else
        {
            try
            {
                string text =  message->chat->firstName+ ", Ваш аккаунт не обнаружен, для идентификации по номеру телефона нажмите на кнопку отправить контакт";
                tgbot.getApi().sendMessage(chatid,text,false, 0,GetRequestPhoneKbd());
            }
            catch (exception& e)
            {
                printf("error: %s\n", e.what());
            }

        }

    }
    else
    {
        isAutorized = true;
    }

    printf("Chat id %lu cmd %s\n",chatid, message->text.c_str());

    if (isAutorized)
    {
        Message::Ptr lastMsg = SendMainMenu(chatid,0,"Бот готов к работе. ");
         if (message->chat->type == Chat::Type::Private)
         {
            if (ListChats[chatid].LastCmd != nullptr)
            {
                tgbot.getApi().deleteMessage(chatid,ListChats[chatid].LastCmd->messageId);
            }
            tgbot.getApi().deleteMessage(chatid,message->messageId);
            ListChats[chatid].LastCmd = lastMsg;
         }

    }



}

InlineKeyboardMarkup::Ptr cnc_bot::GetMainMenu()
{
   InlineKeyboardMarkup::Ptr result(new InlineKeyboardMarkup);

   if (ListDevices.count() >0)
   {
           vector<InlineKeyboardButton::Ptr> row0;

           auto devIt = ListDevices.constBegin();

           while (devIt != ListDevices.constEnd())
           {
                InlineKeyboardButton::Ptr newButton(new InlineKeyboardButton);
                newButton->text = devIt.value().EquipmentName.toStdString();
                newButton->callbackData = Query_device_str.toStdString()+devIt.key().toStdString();;
                row0.push_back(newButton);

                devIt.operator++();

           }
            result->inlineKeyboard.push_back(row0);
   }
   vector<InlineKeyboardButton::Ptr> row1;
   InlineKeyboardButton::Ptr newButton(new InlineKeyboardButton);
   newButton->text = "Обновить";
   newButton->callbackData = Query_main_menu_str.toStdString();
   row1.push_back(newButton);



   result->inlineKeyboard.push_back(row1);
   return result;
}

QString cnc_bot::GetDeviceText(QString aDevice)
{
   QString result;


   result += ListCfgLabel[cfg_max_current]+"- "+ QString::number(ListDevices[aDevice].MaxCurrent)+"\n";
   result += ListCfgLabel[cfg_delta]+"- "+ QString::number(ListDevices[aDevice].Delta)+"\n";
   result += ListCfgLabel[cfg_version]+"- "+ ListDevices[aDevice].FWVersion+"\n";
   result += ListCfgLabel[cfg_mac]+"- "+ aDevice+"\n";
   result += ListCfgLabel[cfg_state]+"- "+ ListStatesDevice[ListDevices[aDevice].State]+"\n";
   result += ListCfgLabel[cfg_description]+"- "+ ListDevices[aDevice].EquipmentName+"\n";
   result += ListCfgLabel[cfg_work_place]+"- "+ ListDevices[aDevice].WorkplaceName+"\n";
   result += ListCfgLabel[сfg_mqtt_id]+"- "+ ListDevices[aDevice].MqttClientID+"\n";

   QString isregistred = "Нет";
   if ( ListDevices[aDevice].isRegistred)
   {
       isregistred = "Да";
   }
   result += ListCfgLabel[cfg_isregistred]+"- "+isregistred+"\n";


   return result;
}
ReplyKeyboardMarkup::Ptr cnc_bot::GetRequestPhoneKbd()
{
    ReplyKeyboardMarkup::Ptr result(new ReplyKeyboardMarkup);
    vector<KeyboardButton::Ptr> row0;
    KeyboardButton::Ptr newButton (new KeyboardButton);
    newButton->text = SendContact_str.toStdString();
    newButton->requestContact = true;
    row0.push_back(newButton);
    result->oneTimeKeyboard = true;
    result->resizeKeyboard = true;
    result->keyboard.push_back(row0);

    return result;
}


InlineKeyboardMarkup::Ptr cnc_bot::GetDeviceKbd(QString aMac)
{
    Q_UNUSED(aMac);

    InlineKeyboardMarkup::Ptr result(new InlineKeyboardMarkup);

    vector<InlineKeyboardButton::Ptr> row0;

    InlineKeyboardButton::Ptr newButton(new InlineKeyboardButton);
    newButton->text = ButtonText_main_menu_str.toStdString();
    newButton->callbackData = Query_main_menu_str.toStdString();

    row0.push_back(newButton);

    result->inlineKeyboard.push_back(row0);

    return result;


}

void cnc_bot::onCallBackQuery(const CallbackQuery::Ptr callbackQuery)
{

    Message::Ptr msg (callbackQuery->message);
    quint64 chatid = msg->chat->id;

    QString query_text = fromUtf8String(callbackQuery->data);


    printf("callback query %s \n",query_text.toStdString().c_str() );

    if (query_text.startsWith(Query_device_str))
    {
        QString device = query_text.remove(Query_device_str);

        ListChats[chatid].curDevice = device;
        QString text = GetDeviceText(device);
        tgbot.getApi().editMessageText(text.toStdString(),chatid,msg->messageId,"","",false,GetDeviceKbd(device));

    }
    else
    if (query_text.startsWith(Query_main_menu_str))
    {
        ListChats[chatid].curDevice = "";
        SendMainMenu(chatid, msg->messageId);
    }

}

void cnc_bot::onInlineQuery(const InlineQuery::Ptr inlineQuery)
{
    printf("inline query %s\n",inlineQuery->query.c_str());


}

void cnc_bot::CheckDevice(QByteArray aMsg)
{
    QJsonDocument doc = QJsonDocument::fromJson(aMsg);
    QJsonObject obj = doc.object();

    auto It = obj.constFind(mac_adress_str);
    if (It != obj.constEnd())
    {
        QString mac = obj[mac_adress_str].toString();
        auto device = ListDevices.constFind(mac);
        if (device == ListDevices.constEnd())
        {
            tDevice newDevice;
            newDevice.EquipmentName = DS.GetDeviceEquipmentName(mac);
            newDevice.WorkplaceName = DS.GetDeviceWorkplaceName(mac);
            newDevice.MqttClientID = DS.GetDeviceMqttClientID(mac);

            if (newDevice.EquipmentName !="")
            {
                newDevice.isRegistred = true;
            }
            ListDevices.insert(mac,newDevice);
        }

        It = obj.constFind(Type_Msg_str);
        if (It != obj.constEnd())
        {
            QString type = obj[Type_Msg_str].toString();
            if (type == ListTypeMsg[tp_msg_info])
            {
                ListDevices[mac].Delta = obj[config_delta_current_str].toString("-1").toInt();
                ListDevices[mac].MaxCurrent = obj[config_max_current_str].toString("-1").toFloat();
                ListDevices[mac].FWVersion = obj[config_version_str].toString("-1");
                ListDevices[mac].State = sd_connected;

            }
            else
            if (type == ListTypeMsg[tp_msg_diag])
            {
                 QString event = obj[Event_str].toString("");
                 if (event !="")
                 {
                     int code_event = ListCodeEventDevice[event];
                     if (code_event<ev_start)
                     {
                        ListDevices[mac].State = code_event;
                     }
                     else
                     {
                        ListDevices[mac].State = sd_connected;
                     }
                 }

            }
        }


    }
}

void cnc_bot::onMqttMessage(QString aNameConnection, const QMQTT::Message &aMessage)
{
    Q_UNUSED(aNameConnection);

    auto msg_array = aMessage.payload();
    auto msg_str = msg_array.toStdString();

    //printf("%s - %s \n",aNameConnection.toStdString().c_str() ,msg_str.c_str());
    CheckDevice(msg_array);

}

void cnc_bot::Init()
{
    ChatClassPtr = this;

    tgbot.getEvents().onCommand("start",&Dispatcher_cmd);
    tgbot.getEvents().onAnyMessage(&Dispatcher_msg);
    tgbot.getEvents().onCallbackQuery(&Dispatcher_callbackquery);
    tgbot.getEvents().onInlineQuery(&Dispatcher_inlineQuery);

    printf("Bot username: %s\n", tgbot.getApi().getMe()->username.c_str());
    tgbot.getApi().deleteWebhook();

    DS.Init();
    mqtt_disp.Init(&Dispatcher_onMqttMessage);
    mqtt_disp.Send(Get_All_Info_str.toLocal8Bit());


    emit poll_thread.operate();
    //printf("End init cnc_bot \n");

}






