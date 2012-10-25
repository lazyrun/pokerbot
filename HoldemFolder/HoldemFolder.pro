TEMPLATE = app
TARGET = HoldemFolder
DESTDIR = bin

include(../CommonSources/common/demo_mode.pri)

CONFIG += debug_and_release
CONFIG -= flat
CONFIG += precompile_header

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = qtincludes.h

QT += phonon
INCLUDEPATH  += ./ gui ../CommonSources/common
DEPENDPATH   += ./ gui ../CommonSources/common

TRANSLATIONS = translations/HoldemFolder_ru.ts \
               translations/HoldemFolder_it.ts \
               translations/HoldemFolder_de.ts \
               translations/HoldemFolder_fr.ts \
               translations/HoldemFolder_es.ts \
               translations/HoldemFolder_ja.ts \
               translations/HoldemFolder_pt.ts

RESOURCES += resources/HoldemFolder.qrc
win32:QMAKE_CXXFLAGS += -MP

# Input
HEADERS += MainWindow.h \
           qtincludes.h \
           MatrixWidget.h \
           SpectrumEditor.h \
           SettingsPage.h \
           OrderPage.h \
           SupportPage.h \
#           VideoPlayer.h \
           SettingsData.h \
           RangesData.h \
           KeyProc.h 
#           aqp.hpp
            
SOURCES += main.cpp \
           MainWindow.cpp \
           MatrixWidget.cpp \
           SpectrumEditor.cpp \
           SettingsPage.cpp \
           OrderPage.cpp \
           SupportPage.cpp \
#           VideoPlayer.cpp \
           SettingsData.cpp \
           RangesData.cpp \           
           KeyProc.cpp            
#           aqp.cpp

win32:RC_FILE = resources/HoldemFolder.rc

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

include(../CommonSources/QtSingleApplication/src/qtsingleapplication.pri)
