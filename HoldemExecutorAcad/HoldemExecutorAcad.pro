TEMPLATE = app
TARGET = HoldemExecutorAcad
DESTDIR = bin

CONFIG += debug_and_release
CONFIG -= flat
CONFIG += precompile_header

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = qtincludes.h

INCLUDEPATH  += ./ src ../CommonSources/common
DEPENDPATH   += ./ src ../CommonSources/common

# Input
HEADERS += Executor.h \
           qtincludes.h \
           ProcAcad.h \
           CardBase.h \
           SettingsData.h \
           AlarmWidget.h \
           Hooker.h

SOURCES += main.cpp \
           Executor.cpp \
           ProcAcad.cpp \
           SettingsData.cpp \
           AlarmWidget.cpp \
           Hooker.cpp

win32:QMAKE_CXXFLAGS += -MP

CONFIG(debug, debug|release) {
     TARGET = $$join(TARGET,,,_d)
     OBJECTS_DIR = obj/Debug
     UI_DIR = obj/Debug/ui
     MOC_DIR = obj/Debug/moc
     RCC_DIR = obj/Debug
     DEPENDPATH += . obj/Debug/ui obj/Debug/moc
} else {
     OBJECTS_DIR = obj/Release
     UI_DIR = obj/Release/ui
     MOC_DIR = obj/Release/moc
     RCC_DIR = obj/Debug
     DEPENDPATH += . obj/Release/ui obj/Release/moc
}

#win32:QMAKE_PRE_LINK = $$quote(cmd python ver.py)

#include(../CommonSources/phash/phash.pri)
include(../CommonSources/img_proc/ImageProc.pri)
#include(src/img_proc/ImageProc.pri)
#include(../CommonSources/QtSingleApplication/src/qtsingleapplication.pri)

DEFINES += EXECUTOR_EXE
include(../CommonSources/GlobalShortcut/GlobalShortcut.pri)
#include(../CommonSources/db/DBManager.pri)
