TEMPLATE = app
TARGET = HoldemExecutorCake
DESTDIR = bin

DEFINES += RESEARCH

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
           ProcCake.h \
           SettingsData.h \
           CardOCR_Cake.h \
           Hooker.h \
           AlarmWidget.h \
           ResearchWindow.h

SOURCES += main.cpp \
           Executor.cpp \
           ProcCake.cpp \
           SettingsData.cpp \
           CardOCR_Cake.cpp \
           Hooker.cpp \
           AlarmWidget.cpp \
           ResearchWindow.cpp

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
