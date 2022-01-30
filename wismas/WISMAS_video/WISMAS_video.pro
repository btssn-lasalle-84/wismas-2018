#-------------------------------------------------
#
# Project created by QtCreator 2018-02-15T17:27:19
#
#-------------------------------------------------

QT       += core gui webkit network

TARGET = WISMAS_video
TEMPLATE = app

OBJECTS_DIR = ./tmp # pour les fichiers objets (.o, .obj)
MOC_DIR = ./tmp # pour les fichiers générés par moc
DESTDIR = ./bin # pour l'exécutable

SOURCES += \
    main.cpp \
    IHMWismas.cpp \
    Camera.cpp \
    Parametrage.cpp \
    Video.cpp \

HEADERS  += \
    IHMWismas.h \
    Camera.h \
    Parametrage.h \
    Video.h \
    Structures.h

FORMS    += \
    IHMWismas.ui

RESOURCES += \
    ressources.qrc
