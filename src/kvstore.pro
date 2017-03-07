TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lboost_coroutine
LIBS += -lboost_context
LIBS += -lboost_filesystem
LIBS += -lboost_thread
LIBS += -lboost_system
LIBS += -lpthread


SOURCES += main.cpp \
    logging/log_recorder.cpp \
    logging/logger.cpp \
    tcp_connection.cpp \
    tcp_server.cpp \
    store_server.cpp \
    kv_database.cpp \
    store_client.cpp \
    cmd_recoder.cpp \
    persistence.cpp \
    recovery.cpp

HEADERS += \
    logging/blocking_queue.h \
    logging/log_recorder.h \
    logging/logger.h \
    logging/singleton.h \
    tcp_connection.h \
    tcp_server.h \
    store_server.h \
    kv_database.h \
    test/test.h \
    test/dotest.h \
    store_client.h \
    cmd_recoder.h \
    persistence.h \
    utils.h \
    recovery.h \
    cmd_protocol.h

