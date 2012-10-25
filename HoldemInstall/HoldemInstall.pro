TEMPLATE = app
TARGET = HoldemInstall
DESTDIR = bin
include(../CommonSources/common/demo_mode.pri)

win32:QMAKE_CXXFLAGS += -MP

CONFIG += debug_and_release static
CONFIG -= flat
CONFIG += precompile_header

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = qtincludes.h

INCLUDEPATH  += ./ gui ../CommonSources/common
DEPENDPATH   += ./ gui ../CommonSources/common

TRANSLATIONS = translations/HoldemInstall_ru.ts \
               translations/HoldemInstall_it.ts \
               translations/HoldemInstall_de.ts \
               translations/HoldemInstall_fr.ts \
               translations/HoldemInstall_es.ts \
               translations/HoldemInstall_ja.ts \
               translations/HoldemInstall_pt.ts

RESOURCES += resources/HoldemInstall.qrc

# Input
HEADERS += Wizard.h qtincludes.h

SOURCES += main.cpp \
           Wizard.cpp

win32:LIBS += Advapi32.lib
#win32:LIBS += -lole32 -luuid
win32:RC_FILE = resources/HoldemInstall.rc
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

include(zip/zip.pri)
