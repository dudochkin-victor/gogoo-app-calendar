### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

include(../common.pri)
TEMPLATE = lib
TARGET = Calendar

QT += declarative
CONFIG += plugin link_pkgconfig
PKGCONFIG += libmkcal

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = $$TARGET
OBJECTS_DIR = .obj
MOC_DIR = .moc

# For building within the tree
INCLUDEPATH += ../lib
LIBS += -L../lib -lmeegocalendar

# Input
SOURCES += \
    components.cpp
HEADERS += \
    components.h

qmldir.files += $$TARGET
qmldir.path += $$[QT_INSTALL_IMPORTS]/MeeGo/App
INSTALLS += qmldir
