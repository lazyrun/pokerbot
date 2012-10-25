INCLUDEPATH  += $$PWD
DEPENDPATH   += $$PWD

INCLUDEPATH += $(QTDIR)/src/3rdparty
# Input
HEADERS += unzip.h \
           unzip_p.h \
           zip.h \
           zip_p.h \
           zipentry_p.h
           
SOURCES += unzip.cpp \
           zip.cpp

