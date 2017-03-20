#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T17:06:01
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QuickViewer
TEMPLATE = app

INCLUDEPATH += ../Qt7z/Qt7z


CONFIG(debug,debug|release) {
LIBS += -L../Qt7z/Qt7z/debug -lQt7z
} else {
LIBS += -L../Qt7z/Qt7z/release -lQt7z
}

SOURCES += main.cpp\
        mainwindow.cpp \
    imageview.cpp \
    filevolume.cpp \
    filevolumedirectory.cpp \
    filevolumeziparchive.cpp \
    filevolume7zarchive.cpp \
    settings.cpp \
    qvapplication.cpp

HEADERS  += mainwindow.h \
    imageview.h \
    filevolume.h \
    filevolumedirectory.h \
    filevolumeziparchive.h \
    filevolume7zarchive.h \
    settings.h \
    qv_init.h \
    qvapplication.h

FORMS    += mainwindow.ui

RESOURCES += \
    toolbar.qrc

RC_ICONS = icons/appicon.ico

quickviewer_dist.path = $$OUT_PWD/dist
quickviewer_dist.files = ./dist/*

INSTALLS += quickviewer_dist

VERSION = 0.1.7
QMAKE_TARGET_COMPANY = KATO Kanryu(k.kanryu@gmail.com)
QMAKE_TARGET_PRODUCT = QuickViewer
QMAKE_TARGET_DESCRIPTION = A image viewer for folders/archives, it can show images very fast
QMAKE_TARGET_COPYRIGHT = (C) 2017 KATO Kanryu

DEFINES += \
  APP_VERSION=\\\"$$VERSION\\\" \
  APP_NAME=\\\"$$QMAKE_TARGET_PRODUCT\\\" \