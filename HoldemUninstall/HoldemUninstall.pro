TEMPLATE = app
TARGET = HoldemRemove
DESTDIR = bin
win32:QMAKE_CXXFLAGS += -MP

CONFIG += debug_and_release
CONFIG -= flat

INCLUDEPATH  += ./ gui ../CommonSources/common
DEPENDPATH   += ./ gui ../CommonSources/common

TRANSLATIONS = translations/HoldemUninstall_ru.ts \
               translations/HoldemUninstall_it.ts \
               translations/HoldemUninstall_de.ts \
               translations/HoldemUninstall_fr.ts \
               translations/HoldemUninstall_es.ts \
               translations/HoldemUninstall_ja.ts \
               translations/HoldemUninstall_pt.ts


RESOURCES += resources/HoldemUninstall.qrc

# Input
HEADERS += Wizard.h 

SOURCES += main.cpp \
           Wizard.cpp

win32:LIBS += Advapi32.lib
win32:RC_FILE = resources/HoldemUninstall.rc
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
