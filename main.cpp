#include <QCoreApplication>
#include <cnc_bot.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //string token(getenv("TOKEN"));
    string token = "5258540400:AAER8tDiPnVCd3sD3dwPmm4ViXkLhTJglXc";

    cnc_bot ChatDisp {token};
    ChatDisp.Init();

    return a.exec();

}
