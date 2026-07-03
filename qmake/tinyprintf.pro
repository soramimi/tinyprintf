TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DESTDIR = $$PWD/../_bin

SOURCES += \
        ../main.c \
        ../tinyprintf.c
