INCLUDEPATH  += $$PWD/src
DEPENDPATH   += $$PWD/src

DEFINES += BUILD_QXT_GUI

HEADERS      += $$PWD/src/qxtglobal.h \
                $$PWD/src/qxtglobalshortcut.h \
                $$PWD/src/qxtglobalshortcut_p.h

SOURCES      += $$PWD/src/qxtglobal.cpp \
                $$PWD/src/qxtglobalshortcut.cpp \
                $$PWD/src/qxtglobalshortcut_win.cpp

