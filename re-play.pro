QT += quick charts qml multimedia
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    core/videoreader.cpp \
    core/videostream.cpp \
    core/timing/clock.cpp \
    core/decoder.cpp \
    utils/qconcurrentqueue.cpp \
    core/video/videodecoder.cpp \
    core/video/videocanvas.cpp \
    rendering/textures/frametexture.cpp \
    rendering/shaders/yuvshader.cpp \
    rendering/geometry/framegeometry.cpp \
    core/audio/audiodecoder.cpp \
    core/audio/speakers.cpp \
    core/audio/speaker.cpp

RESOURCES += qml.qrc \
    data.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = \
    fontawesome/FontAwesome.qml \
    fontawesome/controls/Button.qml \
    fontawesome/controls/Loader.qml \
    fontawesome/controls/Text.qml \
    fontawesome/controls/Variables.qml \

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    core/videoreader.h \
    core/videostream.h \
    core/timing/clock.h \
    core/decoder.h \
    utils/qconcurrentqueue.h \
    core/video/videodecoder.h \
    core/video/videocanvas.h \
    rendering/textures/frametexture.h \
    rendering/shaders/yuvshader.h \
    rendering/textures/frametextures.h \
    rendering/geometry/framegeometry.h \
    core/audio/audiodecoder.h \
    core/audio/speakers.h \
    core/audio/speaker.h

macx: LIBS += \
    -L$$PWD/avlibs/bin/ -lavcodec.58	\
    -L$$PWD/avlibs/bin/ -lavdevice.58	\
    -L$$PWD/avlibs/bin/ -lavfilter.7	\
    -L$$PWD/avlibs/bin/ -lavformat.58	\
    -L$$PWD/avlibs/bin/ -lavutil.56	\
    -L$$PWD/avlibs/bin/ -lpostproc.55	\
    -L$$PWD/avlibs/bin/ -lswresample.3	\
    -L$$PWD/avlibs/bin/ -lswscale.5

win32: LIBS += \
    -L$$PWD/avlibs/bin/ -lavcodec	\
    -L$$PWD/avlibs/bin/ -lavdevice	\
    -L$$PWD/avlibs/bin/ -lavfilter	\
    -L$$PWD/avlibs/bin/ -lavformat	\
    -L$$PWD/avlibs/bin/ -lavutil        \
    -L$$PWD/avlibs/bin/ -lpostproc	\
    -L$$PWD/avlibs/bin/ -lswresample	\
    -L$$PWD/avlibs/bin/ -lswscale

INCLUDEPATH += $$PWD/avlibs/include
DEPENDPATH += $$PWD/avlibs/include

DISTFILES += \
    ../Font Awesome 5 Free-Solid-900.otf \
    resources/fonts/fa_iconsjson2qml.py \
    qml/qmlchart/*

target.path = $$[QT_INSTALL_EXAMPLES]/charts/qmlchart
INSTALLS += target
