TEMPLATE = lib
TARGET = Gammaray

PROVIDER = KDAB

include(../../qtcreatorplugin.pri)
include(../../plugins/analyzerbase/analyzerbase.pri)
include(../../plugins/coreplugin/coreplugin.pri)

LIBS += -L$$IDE_PLUGIN_PATH/Nokia

QT += network 

# Gammaray files

SOURCES += \
    gammarayplugin.cpp \
    gammaraytool.cpp \
    gammarayengine.cpp \
    gammaraysettings.cpp \
    gammarayconfigwidget.cpp

HEADERS += \
    gammarayplugin.h \
    gammaraytool.h \
    gammarayengine.h \
    gammarayconstants.h \
    gammaraysettings.h \
    gammarayconfigwidget.h

FORMS += \
    gammarayconfigwidget.ui \
    gammarayversion.ui
