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

bool cnc_bot::CheckNewText(uint64_t aChatid, QString aNewText)
{
    bool result = false;
    QString old_text;
    if ((ListChats.constFind(aChatid) != ListChats.constEnd())
         && (ListChats[aChatid].LastMsg != nullptr))
        {
            old_text = fromUtf8String(ListChats[aChatid].LastMsg->text);
        }
    result = !(old_text == aNewText) ;
    return result;
}
Message::Ptr cnc_bot::SendMainMenu(uint64_t aChatid, int aMsgId, QString aText)
{
    Message::Ptr result;
    if (ListDevices.count() > 0)
    {
        aText += "Выберите устройство из cписка. ";
    }
    else
    {
        aText += "Устройства не найдены. ";
    }
    aText += "("+QTime::currentTime().toString("HH:mm:ss:zzz")+")";

    if (aMsgId == 0)
    {
        result = tgbot.getApi().sendMessage(aChatid,aText.toStdString(),false, 0, GetMainMenu(),"Markdown");
    }
    else
    {
        if (CheckNewText(aChatid,aText))
        {
            result = tgbot.getApi().editMessageText(aText.toStdString(),aChatid, aMsgId,"","Markdown",false,GetMainMenu());
        }


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
                printf("Error request contact: %s\n", e.what());
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
           vector<InlineKeyboardButton::Ptr> row_devices;

           auto devIt = ListDevices.constBegin();
           int devcount = 0;
           while (devIt != ListDevices.constEnd())
           {
                InlineKeyboardButton::Ptr newButton(new InlineKeyboardButton);
                QString button_text = "";
                if ((devIt.value().MeshLayer >0) & (devIt->State == sd_connected))
                {
                   button_text += QString::number(devIt.value().MeshLayer)+delim;
                }
                if (devIt->isRegistred)
                {
                    button_text += devIt.value().EquipmentName;
                }
                else
                {
                    button_text += devIt.value().MqttClientID;
                }

                if (devIt->State == sd_disconnected)
                    { button_text += " *";}
                else
                    if (devIt->State == sd_pause)
                        { button_text += " ||";}

                newButton->text = button_text.toStdString();
                newButton->callbackData = Query_device_str.toStdString()+devIt.key().toStdString();
                row_devices.push_back(newButton);

                devcount++;
                if (( devcount % 2 == 0) | (devcount == ListDevices.count()))
                {
                    result->inlineKeyboard.push_back(row_devices);
                    row_devices.clear();
                }

                devIt.operator++();


           }

   }
   vector<InlineKeyboardButton::Ptr> row1;
   InlineKeyboardButton::Ptr newButton(new InlineKeyboardButton);
   newButton->text = ButtonText_refresh_str.toStdString();
   newButton->callbackData = Query_main_menu_str.toStdString();
   row1.push_back(newButton);



   result->inlineKeyboard.push_back(row1);
   return result;
}

QString cnc_bot::GetDeviceText(QString aDevice)
{
   QString result;

   result += QDateTime::currentDateTime().toString("MM:dd:yyyy HH:mm:ss")+ret;
   result += ListCfgLabel[cfg_max_current]+delim+ QString::number(ListDevices[aDevice].MaxCurrent)+ret;
   result += ListCfgLabel[cfg_delta]+delim+ QString::number(ListDevices[aDevice].Delta)+ret;
   result += ListCfgLabel[cfg_version]+delim+ ListDevices[aDevice].FWVersion+ret;
   result += ListCfgLabel[cfg_mac]+delim+ aDevice+ret;
   result += ListCfgLabel[cfg_state]+delim+ ListStatesDevice[ListDevices[aDevice].State]+ret;
   result += ListCfgLabel[cfg_description]+delim+ ListDevices[aDevice].EquipmentName+ret;
   result += ListCfgLabel[cfg_work_place]+delim+ ListDevices[aDevice].WorkplaceName+ret;
   result += ListCfgLabel[сfg_mqtt_id]+delim+ ListDevices[aDevice].MqttClientID+ret;
   result += ListCfgLabel[cfg_free_ram]+delim+ QString::number(ListDevices[aDevice].FreeRam)+ret;
   result += ListCfgLabel[cfg_current]+delim+ QString::number(ListDevices[aDevice].LastCurrent)+ret;
   result += ListCfgLabel[cfg_currentTS]+delim+ QDateTime::fromMSecsSinceEpoch(ListDevices[aDevice].LastCurrentTS).toString("HH:mm:ss.zzz")+ret;
   result += ListCfgLabel[cfg_mesh_layer]+delim+ QString::number(ListDevices[aDevice].MeshLayer)+ret;
   QString isregistred = "Нет";
   if ( ListDevices[aDevice].isRegistred)
   {
       isregistred = "Да";
   }
   result += ListCfgLabel[cfg_isregistred]+delim+isregistred;


   return result;
}
ReplyKeyboardMarkup::Ptr cnc_bot::GetRequestPhoneKbd()
{
    ReplyKeyboardMarkup::Ptr result(new ReplyKeyboardMarkup);
    vector<KeyboardButton::Ptr> row0;
    KeyboardButton::Ptr newButton (new KeyboardButton);
    newButton->text = ButtonText_SendContact_str.toStdString();
    newButton->requestContact = true;
    row0.push_back(newButton);
    result->oneTimeKeyboard = true;
    result->resizeKeyboard = true;
    result->keyboard.push_back(row0);

    return result;
}


InlineKeyboardMarkup::Ptr cnc_bot::GetDeviceKbd(QString aMac)
{

    InlineKeyboardMarkup::Ptr result(new InlineKeyboardMarkup);

    auto CmdIt = ListCmd.constBegin();

    if (ListDevices[aMac].State != sd_disconnected)
    {
        while (CmdIt != ListCmd.constEnd())
        {
            vector<InlineKeyboardButton::Ptr> cmd_row;
            InlineKeyboardButton::Ptr cmdButton(new InlineKeyboardButton);
            cmdButton->text = ListCmdLabel[CmdIt.key()].toStdString();
            cmdButton->callbackData = (CmdIt.value()+query_delim+aMac).toStdString();
            cmd_row.push_back(cmdButton);
            result->inlineKeyboard.push_back(cmd_row);
            cmd_row.clear();
            CmdIt.operator++();
        }
    }

    vector<InlineKeyboardButton::Ptr> end_row;
    InlineKeyboardButton::Ptr mmenuButton(new InlineKeyboardButton);
    mmenuButton->text = ButtonText_main_menu_str.toStdString();
    mmenuButton->callbackData = Query_main_menu_str.toStdString();
    end_row.push_back(mmenuButton);

    result->inlineKeyboard.push_back(end_row);

    return result;


}

void cnc_bot::SendDevMenu(uint64_t aChatid, QString aTypeMsg)
{
    QString device = ListChats[aChatid].curDevice;

    qint64 deltaTime =  QDateTime::currentMSecsSinceEpoch() - ListChats[aChatid].LastSendTS;
    if ( ( deltaTime < SendMsgIntervalMin  )
            & (ListChats[aChatid].LastSendTS != 0)
            & ((aTypeMsg == ListTypeMsg[tp_msg_data]) | (aTypeMsg == "")) )
    {
        printf("Привышена частота отправки данных в чат (%lld)\n",deltaTime);
    }
    else
    {
        QString text = GetDeviceText(device);
        QString old_text;
        if (CheckNewText(aChatid,text))
        {
            try
            {
                Message::Ptr lastMsg = tgbot.getApi().editMessageText(text.toStdString(),aChatid,ListChats[aChatid].LastMsg->messageId,"","Markdown",false,GetDeviceKbd(device));
                if (lastMsg != nullptr)
                {
                    ListChats[aChatid].LastMsg = lastMsg;
                    if ((aTypeMsg == ListTypeMsg[tp_msg_data]) | (aTypeMsg == ""))
                    {
                        ListChats[aChatid].LastSendTS = QDateTime::currentMSecsSinceEpoch();
                    }
                }
                else
                {
                    printf("%s - отправка меню не удалась\n",QTime::currentTime().toString("HH:mm:ss.zzz").toStdString().c_str());
                }
            }
            catch (exception& e)
            {
                printf("Исключение при отправке меню устройства:\n %s\n", e.what());

            }
        }
    }

}


void cnc_bot::onCallBackQuery(const CallbackQuery::Ptr callbackQuery)
{

    Message::Ptr msg (callbackQuery->message);
    quint64 chatid = msg->chat->id;
    tChat chat;
    if (ListChats.find(chatid) == ListChats.constEnd())
    {
        ListChats.insert(chatid,chat);
    }

    QString query_text = fromUtf8String(callbackQuery->data);
    //printf("callback query %s \n",query_text.toStdString().c_str() );

    //выбор устройства
    if (query_text.startsWith(Query_device_str))
    {
        QString device = query_text.remove(Query_device_str);

        ListChats[chatid].curDevice = device;
        ListChats[chatid].LastMsg = msg;
        SendDevMenu(chatid);
    }
    else
    // главное меню
    if (query_text.startsWith(Query_main_menu_str))
    {
        ListChats[chatid].curDevice = "";
        SendMainMenu(chatid, msg->messageId);
    }
    else
    // команды устройству
    if (query_text.startsWith(CMD_str))
    {
        QJsonDocument json_doc;
        QJsonObject json_obj;
        QJsonValue val(CMD_str);
        QString cmd,mac;
        auto cmd_items = query_text.split(query_delim);
        if (cmd_items.count() == 2)
        {
            cmd = cmd_items[0];
            mac = cmd_items[1];

            json_obj.insert(Type_Msg_str,val);
            val = mac;
            json_obj.insert(ListCfg[cfg_mac],val);
            val = cmd;
            json_obj.insert(CMD_str,val);

            if (cmd == ListCmd[cmd_flash])
            {
                val = OTA_URL;
                json_obj.insert(OTA_URL_str,val);
            }


            if (cmd !=ListCmd[cmd_set_config])
            {
                json_doc = QJsonDocument(json_obj);
                mqtt_disp.Send(json_doc.toJson());
            }


        }

    }


}

void cnc_bot::onInlineQuery(const InlineQuery::Ptr inlineQuery)
{
    printf("inline query %s\n",inlineQuery->query.c_str());


}

void cnc_bot::CheckDevice(QByteArray aMsg)
{
    QJsonDocument json_doc = QJsonDocument::fromJson(aMsg);
    QJsonObject json_obj = json_doc.object();



    if (json_obj.constFind(ListCfg[cfg_mac]) != json_obj.constEnd())
    {
        QString mac = json_obj[ListCfg[cfg_mac]].toString();
        QString type;
        if (json_obj.constFind(Type_Msg_str) != json_obj.constEnd())
        {
            type = json_obj[Type_Msg_str].toString();
            // INFO
            if (type == ListTypeMsg[tp_msg_info])
            {
                //printf("%s \n",aMsg.toStdString().c_str());
                AddDevice(mac);
                ListDevices[mac].Delta = json_obj[ListCfg[cfg_delta]].toString("").toInt();
                ListDevices[mac].MaxCurrent = json_obj[ListCfg[cfg_max_current]].toString("").toFloat();
                ListDevices[mac].FWVersion = json_obj[ListCfg[cfg_version]].toString("");
                ListDevices[mac].MqttClientID = json_obj[ListCfg[сfg_mqtt_id]].toString("");
                if (ListDevices[mac].State == sd_disconnected )
                    {ListDevices[mac].State = sd_connected;}
                ListDevices[mac].LastInfoTS = QDateTime::currentMSecsSinceEpoch();
                ListDevices[mac].FreeRam= json_obj[ListCfg[cfg_free_ram]].toString("").toInt();
                ListDevices[mac].MeshLayer= json_obj[ListCfg[cfg_mesh_layer]].toString("-1").toInt();;

            }
            else
            // DIAG
            if (type == ListTypeMsg[tp_msg_diag])
            {
                 QString event = json_obj[Event_str].toString("");
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
                     QString ts = QTime::currentTime().toString();
                     printf("%s = %s - %s\n",ts.toStdString().c_str(),mac.toStdString().c_str(), event.toStdString().c_str());
                 }

            }
            else
            // DATA
            if (type == ListTypeMsg[tp_msg_data])
            {
                ListDevices[mac].LastCurrent =  json_obj[current_str].toString("0").toFloat();
                ListDevices[mac].LastCurrentTS = json_obj[TS_str].toString("0").toLongLong();

            }
        }
        auto ChatIt = ListChats.constBegin();
        while (ChatIt != ListChats.constEnd())
        {
            if (ChatIt->curDevice == mac)
            {
                SendDevMenu(ChatIt.key(),type);
            }
            else
            if (type == ListTypeMsg[tp_msg_info])
            {
                if (ChatIt->LastMsg != nullptr)
                {SendMainMenu(ChatIt->LastMsg->chat->id,ChatIt->LastMsg->messageId);}
            }

            ChatIt.operator++();
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
void cnc_bot::AddDevice(QString aMac, int aState)
{
    if (ListDevices.constFind(aMac) == ListDevices.constEnd())
    {
        tDevice newDevice;
        newDevice.EquipmentName = DS.GetDeviceEquipmentName(aMac);
        newDevice.WorkplaceName = DS.GetDeviceWorkplaceName(aMac);
        newDevice.State = aState;

        if (newDevice.EquipmentName != "")
            {newDevice.isRegistred = true;}
        ListDevices.insert(aMac,newDevice);
    }
    else
    {
        ListDevices[aMac].EquipmentName = DS.GetDeviceEquipmentName(aMac);
        ListDevices[aMac].WorkplaceName = DS.GetDeviceWorkplaceName(aMac);
    }

}
void cnc_bot::BuildDevicesList()
{
    QStringList devices = DS.GetListDevicesMac();

    for (int index = 0;index<devices.count(); index++)
    {
        AddDevice( devices[index]);
    }

}
void cnc_bot::ReadConfig()
{
    if ( (Config.isParsedOK()) &  Config.SelectConfig(ConfigName_str)  )
    {
        tAttributes CurSetAttrib = Config.GetPlaceFirst(Place_str);
        bool EoSets= (CurSetAttrib.count() == 0);
        // Ищем в конфиге наборы установок
        while (!EoSets)
        {
            OTA_URL = CurSetAttrib.value(OTA_URL_tag_str,"").toString();

            CurSetAttrib = Config.GetPlaceNext();
            EoSets= (CurSetAttrib.count() == 0);
        }
    }
}

void cnc_bot::Init()
{
    ChatClassPtr = this;

    ReadConfig();

    PingDeviceTimer.setInterval(PingDeviceInteval);
    connect(&PingDeviceTimer, &QTimer::timeout, this, &cnc_bot::onPingTimer);


    tgbot.getEvents().onCommand("start",&Dispatcher_cmd);
    tgbot.getEvents().onAnyMessage(&Dispatcher_msg);
    tgbot.getEvents().onCallbackQuery(&Dispatcher_callbackquery);
    tgbot.getEvents().onInlineQuery(&Dispatcher_inlineQuery);

    printf("Bot username: %s\n", tgbot.getApi().getMe()->username.c_str());
    tgbot.getApi().deleteWebhook();

    DS.Init();

    mqtt_disp.Init(&Dispatcher_onMqttMessage);
    BuildDevicesList();

    PingDeviceTimer.start();

    emit poll_thread.operate();
    //printf("End init cnc_bot \n");

}

void cnc_bot::onPingTimer()
{
    auto devIt = ListDevices.constBegin();
    int devcount = 0;
    while (devIt != ListDevices.constEnd())
    {
         if ((QDateTime::currentMSecsSinceEpoch() - devIt.value().LastInfoTS) >TimeOutDevice)
         {
             QString ts = QTime::currentTime().toString();
             if (ListDevices[devIt.key()].State == sd_connected)
             {
                 printf("%s = %s timeout \n",ts.toStdString().c_str(), devIt.key().toStdString().c_str());
             }
             ListDevices[devIt.key()].State = sd_disconnected;
         }

         devcount++;
         devIt.operator++();


    }
    mqtt_disp.Send(Get_All_Info_str.toLocal8Bit());
}




