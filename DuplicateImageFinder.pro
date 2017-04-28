#-------------------------------------------------
#
# Project created by QtCreator 2016-10-05T15:30:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DuplicateImageFinder
TEMPLATE = app

debug {
  DEFINES += MYPREFIX_DEBUG
}

DEFINES += QT_USE_QSTRINGBUILDER
CONFIG += c++11

unix: {
    #LIBS += -lopencv_highgui -lopencv_core -lopencv_features2d -lopencv_calib3d -lopencv_flann -lopencv_nonfree
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

win32: {
    QT += winextras
    DEFINES += USE_WIN_EXTRAS
}

win32-g++: {
    INCLUDEPATH+=C:\\OpenCV\\2.4.13\\opencv\\build\\include
    INCLUDEPATH+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin
    INCLUDEPATH+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib
    CONFIG(debug, debug|release) {
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_highgui2413d.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_core2413d.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_imgproc2413d.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_features2d2413d.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_objdetect2413d.dll.a
LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_nonfree2413d.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_highgui2413d.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_core2413d.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_imgproc2413d.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_features2d2413d.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_objdetect2413d.dll
LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_nonfree2413d.dll
    } else {
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_highgui2413.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_core2413.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_imgproc2413.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_features2d2413.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_objdetect2413.dll.a
LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\lib\\libopencv_nonfree2413.dll.a
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_highgui2413.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_core2413.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_imgproc2413.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_features2d2413.dll
        LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_objdetect2413.dll
LIBS+=C:\\OpenCV\\2.4.13\\opencv\\build\\bin\\libopencv_nonfree2413.dll
    }
}

SOURCES += src/main.cpp\
        src/gui/mainwindow.cpp \
    src/gui/dialog/filtersdialog.cpp \
    src/worker/dirwalker.cpp \
    src/worker/duplicatefinder.cpp \
    src/model/basetablemodel.cpp \
    src/model/duplicatestablemodel.cpp \
    src/settingshelper.cpp \
    src/worker/duplacatehistogramfinder.cpp \
    src/worker/duplacatehistogramqfinder.cpp \
    src/gui/dialog/settingsdialog.cpp \
    src/worker/diplicatesiftdistancefinder.cpp

HEADERS  += src/gui/mainwindow.h \
    src/hashfileinfostruct.h \
    src/gui/dialog/filtersdialog.h \
    src/worker/dirwalker.h \
    src/worker/duplicatefinder.h \
    src/model/basetablemodel.h \
    src/model/duplicatestablemodel.h \
    src/settingshelper.h \
    src/worker/duplacatehistogramfinder.h \
    src/constants.h \
    src/worker/duplacatehistogramqfinder.h \
    src/gui/dialog/settingsdialog.h \
    src/worker/diplicatesiftdistancefinder.h

FORMS    += src/gui/mainwindow.ui \
    src/gui/dialog/settingsdialog.ui

VERSION = 0.0.0.2
QMAKE_TARGET_PRODUCT = DuplicateImageFinder
QMAKE_TARGET_DESCRIPTION = "Program for find duplicates or images."
QMAKE_TARGET_COPYRIGHT = (c) Yuriy Astrov
# RC_ICONS = myappico.ico
# RC_FILE = myapp.rc
GITHUB_URL = "https://github.com/yastrov/DuplicateImageFinder"

DEFINES += \
APP_VERSION=\"\\\"$$VERSION\\\"\" \
APP_URL=\"\\\"$$GITHUB_URL\\\"\" \
