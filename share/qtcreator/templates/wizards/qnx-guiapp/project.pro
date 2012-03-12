QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

SOURCES += main.cpp mainwidget.cpp
HEADERS += mainwidget.h
FORMS   += mainwidget.ui

OTHER_FILES += bar-descriptor.xml
