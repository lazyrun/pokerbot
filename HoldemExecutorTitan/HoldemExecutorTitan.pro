TEMPLATE = app
TARGET = HoldemExecutorTitan
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
           ProcTitan.h \
           SettingsData.h \
           CardOCR_Titan.h \
           Hooker.h \
           AlarmWidget.h

SOURCES += main.cpp \
           Executor.cpp \
           ProcTitan.cpp \
           SettingsData.cpp \
           CardOCR_Titan.cpp \
           Hooker.cpp \
           AlarmWidget.cpp

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

include(../CommonSources/img_proc/ImageProc.pri)

DEFINES += EXECUTOR_EXE
include(../CommonSources/GlobalShortcut/GlobalShortcut.pri)
