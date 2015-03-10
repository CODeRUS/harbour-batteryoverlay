TARGET = harbour-batteryoverlay

QT += dbus gui-private
CONFIG += link_pkgconfig sailfishapp
PKGCONFIG += mlite5

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/main.cpp \
    src/viewhelper.cpp

HEADERS += \
    src/viewhelper.h

OTHER_FILES += \
    rpm/harbour-batteryoverlay.spec \
    harbour-batteryoverlay.desktop \
    harbour-batteryoverlay.png \
    qml/overlay.qml \
    qml/settings.qml \
    qml/pages/MainPage.qml \
    qml/cover/CoverPage.qml \
    qml/pages/ColorDialog.qml \
    qml/components/ColorSelector.qml \
    qml/components/ColorItem.qml \
    qml/pages/AboutPage.qml
