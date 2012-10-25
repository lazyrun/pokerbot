TEMPLATE = lib
TARGET = cake
DESTDIR = bin

DEFINES += MAKE_EXEC_DLL
DEFINES += _CRT_SECURE_NO_DEPRECATE

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
           cakelib.h \
           Hooker.h \
           AlarmWidget.h

SOURCES += Executor.cpp \
           ProcCake.cpp \
           SettingsData.cpp \
           CardOCR_Cake.cpp \
           cakelib.cpp \
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
