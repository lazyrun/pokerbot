TEMPLATE = app
TARGET = HoldemAgent
DESTDIR = bin

include(../CommonSources/common/demo_mode.pri)

CONFIG += debug_and_release
CONFIG -= flat
CONFIG += precompile_header

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = qtincludes.h

INCLUDEPATH  += ./ src ../CommonSources/common
DEPENDPATH   += ./ src ../CommonSources/common

TRANSLATIONS = translations/HoldemAgent_ru.ts \
               translations/HoldemAgent_it.ts \
               translations/HoldemAgent_de.ts \
               translations/HoldemAgent_fr.ts \
               translations/HoldemAgent_es.ts \
               translations/HoldemAgent_ja.ts \
               translations/HoldemAgent_pt.ts

RESOURCES += resources/HoldemAgent.qrc

# Input
HEADERS += Agent.h \
           qtincludes.h \
           SettingsData.h \
           RangesData.h

SOURCES += main.cpp \
           Agent.cpp \
           SettingsData.cpp \
           RangesData.cpp
           
win32:RC_FILE = resources/HoldemAgent.rc
win32:DEFINES += _CRT_SECURE_NO_DEPRECATE

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

include(../CommonSources/QtSingleApplication/src/qtsingleapplication.pri)
include(../CommonSources/GlobalShortcut/GlobalShortcut.pri)
