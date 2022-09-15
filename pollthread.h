#ifndef POLLTHREAD_H
#define POLLTHREAD_H

#include <QThread>
#include <tgbot/Bot.h>
#include <tgbot/EventHandler.h>
#include <tgbot/net/TgLongPoll.h>

using namespace std;
using namespace TgBot;

class Worker : public QObject
{
    Q_OBJECT

public :

    Worker(const Bot& bot): longPoll (bot)
    {

    };

    void start() {

        while(true)
        {
            try
            {
                longPoll.start();
                emit resultReady();
            }
            catch (exception& e)
            {
                printf("Poll send error: %s\n", e.what());
            }

        }
    }

    TgLongPoll longPoll;

signals:
    void resultReady();
};



class PollThread: public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    PollThread(const Bot& bot): worker (bot)
    {
        worker.moveToThread(&workerThread);
        connect(this, &PollThread::operate, &worker, &Worker::start);
        connect(&worker, &Worker::resultReady, this, &PollThread::handleResults);
        workerThread.start();
        //printf("Controller started \n");

    };
    ~PollThread()
    {
        workerThread.quit();
        workerThread.wait();
    }
    Worker worker;
signals:
    void operate();
public slots:
    void handleResults();
};

#endif // POLLTHREAD_H
