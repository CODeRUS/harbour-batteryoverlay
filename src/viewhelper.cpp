#include "viewhelper.h"

#include <qpa/qplatformnativeinterface.h>
#include <QDebug>

ViewHelper::ViewHelper(QObject *parent) :
    QObject(parent)
{
    view = NULL;
}

void ViewHelper::closeOverlay()
{
    QDBusInterface iface("harbour.batteryoverlay.overlay", "/", "harbour.batteryoverlay");
    iface.call(QDBus::NoBlock, "exit");
}

void ViewHelper::checkActive()
{
    bool inactive = QDBusConnection::sessionBus().registerService("harbour.batteryoverlay.overlay");
    if (inactive) {
        showOverlay();
    }
    else {
        bool newSettings = QDBusConnection::sessionBus().registerService("harbour.batteryoverlay.settings");
        if (newSettings) {
            showSettings();
        }
        else {
            QDBusInterface iface("harbour.batteryoverlay.settings", "/", "harbour.batteryoverlay");
            iface.call(QDBus::NoBlock, "show");
            qGuiApp->exit(0);
            return;
        }
    }
    QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportScriptableSlots);

    QDBusConnection::sessionBus().connect("", "", "com.jolla.jollastore", "packageStatusChanged", this, SLOT(onPackageStatusChanged(QString, int)));
}

void ViewHelper::show()
{
    if (view) {
        view->raise();
    }
}

void ViewHelper::exit()
{
    qGuiApp->exit(0);
}

void ViewHelper::showOverlay()
{
    qGuiApp->setApplicationName("Battery Overlay");
    qGuiApp->setApplicationDisplayName("Battery Overlay");

    view = SailfishApp::createView();
    view->setTitle("BatteryOverlay");

    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    view->setColor(color);
    view->setClearBeforeRendering(true);

    view->show();
    view->close();

    view->setSource(SailfishApp::pathTo("qml/overlay.qml"));
    view->create();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(view->handle(), QLatin1String("CATEGORY"), "notification");
    native->setWindowProperty(view->handle(), QLatin1String("MOUSE_REGION"), QRegion(0, 0, 0, 0));

    view->showFullScreen();
}

void ViewHelper::showSettings()
{
    qGuiApp->setApplicationName("Battery Overlay Settings");
    qGuiApp->setApplicationDisplayName("Battery Overlay Settings");

    view = SailfishApp::createView();
    view->setTitle("BatteryOverlay Settings");
    view->rootContext()->setContextProperty("helper", this);
    view->setSource(SailfishApp::pathTo("qml/settings.qml"));
    view->showFullScreen();
}

void ViewHelper::onPackageStatusChanged(const QString &package, int status)
{
    if (package == "harbour-batteryoverlay") {
        if (status != 1) {
            exit();
        }
    }
}
