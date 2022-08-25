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
void cnc_bot::onCmd(const Message::Ptr message)
{
    uint64_t chatid = message->chat->id;
    tChat chat;

    if (ListChats.find(chatid) == ListChats.constEnd())
    {
        ListChats.insert(chatid,chat);
    }

    printf("Chat id %lu cmd %s\n",chatid, message->text.c_str());
    QString text = "Бот готов к работе";

    Message::Ptr lastMsg = tgbot.getApi().sendMessage(chatid,text.toStdString(),false, 0, GetMainMenu());
    if (ListChats[chatid].LastMsg != nullptr)
    {

        tgbot.getApi().deleteMessage(chatid,ListChats[chatid].LastMsg->messageId);
    }
    tgbot.getApi().deleteMessage(chatid,message->messageId);
    ListChats[chatid].LastMsg = lastMsg;


}
/**
 * @brief onMsg
 * @param message
 */
void cnc_bot::onMsg(const Message::Ptr message)
{
    if (!StringTools::startsWith(message->text,"/start") )
    {
        printf("User wrote %s\n", message->text.c_str());
        tgbot.getApi().deleteMessage(message->chat->id ,message->messageId);
    }
}

void cnc_bot::onCallBackQuery(const CallbackQuery::Ptr callbackQuery)
{
    printf("callback query %s %d \n",callbackQuery->data.c_str(), callbackQuery->message->messageId);
    Message::Ptr msg (callbackQuery->message);

    /*
    string newtext = "Кнопка нажата пользователем с именем: "+callbackQuery->from->firstName+" "+QDateTime::currentDateTime().toString().toStdString() ;
    printf("from user %ld,%s %s %s \n",callbackQuery->from->id, callbackQuery->from->firstName.c_str(), callbackQuery->from->lastName.c_str(), callbackQuery->from->username.c_str());
    */
    //if (callbackQuery->data == StrCmdList.toStdString())
    {
        tgbot.getApi().editMessageText("1",msg->chat->id,msg->messageId,"","",false);//,kbdDevices);
    }


    //tgbot.getApi().answerCallbackQuery(callbackQuery->id, callbackQuery->from->firstName+" cпасибо что нажали кнопку",true);
}
void cnc_bot::onInlineQuery(const InlineQuery::Ptr inlineQuery)
{
    printf("inline query %s\n",inlineQuery->query.c_str());


}

void cnc_bot::CheckDevice(QByteArray aMsg)
{
    QJsonDocument doc = QJsonDocument::fromJson(aMsg);
    QJsonObject obj = doc.object();
}

void cnc_bot::onMqttMessage(QString aNameConnection, const QMQTT::Message &aMessage)
{
    auto msg_array = aMessage.payload();
    auto msg_str = aMessage.payload().toStdString();
    printf("%s - %s \n",aNameConnection.toStdString().c_str() ,msg_str.c_str());

    //auto mac = obj[];
}

void cnc_bot::Init()
{
    ChatClassPtr = this;

    QObject::connect(&BotUpdateTimer, &QTimer::timeout, this, &cnc_bot::StartPoll);
    BotUpdateTimer.setInterval(BotUpdateInterval*1000);

    tgbot.getEvents().onCommand("start",&Dispatcher_cmd);
    tgbot.getEvents().onAnyMessage(&Dispatcher_msg);
    tgbot.getEvents().onCallbackQuery(&Dispatcher_callbackquery);
    tgbot.getEvents().onInlineQuery(&Dispatcher_inlineQuery);

    printf("Bot username: %s\n", tgbot.getApi().getMe()->username.c_str());
    tgbot.getApi().deleteWebhook();

    BotUpdateTimer.start();
    mqtt_disp.Init(&Dispatcher_onMqttMessage);


    QString test_text = "{\"TypeMsg\":\"CMD\",\"mac_address\":\"ALL\",\"CMD\":\"CMD_GET_CONFIG\"}";
    mqtt_disp.Send(test_text.toLocal8Bit());
}

InlineKeyboardMarkup::Ptr cnc_bot::GetMainMenu()
{
   InlineKeyboardMarkup::Ptr result(new InlineKeyboardMarkup);

   vector<InlineKeyboardButton::Ptr> row0;
   InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
   checkButton->text = "check";
   checkButton->callbackData = "check";
   row0.push_back(checkButton);
   result->inlineKeyboard.push_back(row0);

   return result;
}

void cnc_bot::StartPoll()
{
    longPoll.start();
}



