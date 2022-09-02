QT += gui
QT += network xml core sql

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += HAVE_CURL
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        cnc_bot.cpp \
        datasource.cpp \
        main.cpp \
        mqttclient.cpp \
        mqttconnectiondispatcher.cpp \
        pollthread.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


unix:!macx: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lTgBot

INCLUDEPATH += $$PWD/../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../usr/local/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/libTgBot.a

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lcrypto

INCLUDEPATH += $$PWD/../../../../../usr/include/openssl
DEPENDPATH += $$PWD/../../../../../usr/include/openssl

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lssl

INCLUDEPATH += $$PWD/../../../../../usr/include/openssl
DEPENDPATH += $$PWD/../../../../../usr/include/openssl

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lboost_system

INCLUDEPATH += $$PWD/../../../../../usr/include/boost
DEPENDPATH += $$PWD/../../../../../usr/include/boost

unix:!macx: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lcurl

INCLUDEPATH += $$PWD/../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../usr/local/include

HEADERS += \
    cnc_bot.h \
    datasource.h \
    mqttclient.h \
    mqttconnectiondispatcher.h \
    pollthread.h

unix:!macx: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lqmqtt

INCLUDEPATH += $$PWD/../../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../../usr/local/include

DISTFILES += \
    config.xml

unix:!macx: LIBS += -L$$PWD/../libs/ConfigXmlLib/ -lConfigXmlLib

INCLUDEPATH += $$PWD/../libs/ConfigXmlLib
DEPENDPATH += $$PWD/../libs/ConfigXmlLib

unix:!macx: PRE_TARGETDEPS += $$PWD/../libs/ConfigXmlLib/libConfigXmlLib.a

unix:!macx: LIBS += -L$$PWD/../libs/LogErrLib/ -lLogErrLib

INCLUDEPATH += $$PWD/../libs/LogErrLib
DEPENDPATH += $$PWD/../libs/LogErrLib

unix:!macx: PRE_TARGETDEPS += $$PWD/../libs/LogErrLib/libLogErrLib.a
