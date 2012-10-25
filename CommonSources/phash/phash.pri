INCLUDEPATH  += $$PWD
DEPENDPATH   += $$PWD

DEFINES += HAVE_IMAGE_HASH

HEADERS      += 	dirent.h \
                  ph_fft.h \
                  pHash.h \
                  phcomplex.h
                  
SOURCES      += 	dirent.c \
                  ph_fft.c \
                  phcomplex.c \
                  pHash.cpp
