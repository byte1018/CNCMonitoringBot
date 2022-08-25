#include <QCoreApplication>
#include <cnc_bot.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //string token(getenv("TOKEN"));
    string token = "5258540400:AAE-bFb_RGGfHAS0TQSGSUEkVt-1l2mHkA0";

    cnc_bot ChatDisp {token};
    ChatDisp.Init();

    return a.exec();

}
