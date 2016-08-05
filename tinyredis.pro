TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11
LIBS += -levent
LIBS += -lpthread
SOURCES += main.cpp \
    multi_thread_server.cc \
    redis-server.cc

DISTFILES += \
    tinyredis.pro.user

HEADERS += \
    db.h \
    multi_thread_server.h \
    redis-server.h \
    utils.h

