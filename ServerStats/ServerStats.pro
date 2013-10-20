TEMPLATE = app
TARGET = ServerStats
DEPENDPATH += . Config shared
INCLUDEPATH += . shared Config

# Input
HEADERS += fileexportsettings.h \
           ftpexportsettings.h \
           resource.h \
           serveredit.h \
           serverstats.h \
           stdafx.h \
           Config/config.h \
           shared/blowfish.h \
           shared/silkroad_security.h \
           shared/stream_utility.h
FORMS += fileexportsettings.ui ftpexportsettings.ui serveredit.ui serverstats.ui
SOURCES += fileexportsettings.cpp \
           ftpexportsettings.cpp \
           main.cpp \
           serveredit.cpp \
           serverstats.cpp \
           stdafx.cpp \
           Config/config.cpp \
           shared/blowfish.cpp \
           shared/silkroad_security.cpp \
           shared/stream_utility.cpp
RESOURCES += serverstats.qrc
QT += widgets network
LIBS += -lboost_system -lboost_thread -lboost_program_options -lboost_filesystem -lboost_serialization -lboost_random
QMAKE_CFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++11
