#-------------------------------------------------
#
# Project created by QtCreator 2018-07-19T10:29:18
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Admin
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    login.cpp \
    databaseoperation.cpp \
    databaseinit.cpp \
    choosefiledialog.cpp \
    ftpmanager.cpp \
    waitingdialog.cpp \
    compressionworker.cpp

HEADERS += \
        mainwindow.h \
    login.h \
    databaseoperation.h \
    databaseinit.h \
    choosefiledialog.h \
    ftpmanager.h \
    waitingdialog.h \
    compressionworker.h

FORMS += \
        mainwindow.ui \
    login.ui \
    waitingdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QLib7z-master/lib/release/ -lQLib7z
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QLib7z-master/lib/debug/ -lQLib7z
else:unix: LIBS += -L$$PWD/QLib7z-master/lib/ -lQLib7z

INCLUDEPATH += $$PWD/QLib7z-master/include
DEPENDPATH += $$PWD/QLib7z-master/include
