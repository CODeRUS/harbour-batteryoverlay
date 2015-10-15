#include "viewhelper.h"
#include "colorhelper.h"

#include <qpa/qplatformnativeinterface.h>
#include <QTimer>
#include <QDebug>
#include <QDBusReply>

ViewHelper::ViewHelper(QObject *parent) :
    QObject(parent),
    overlayView(NULL),
    settingsView(NULL)
{
    QDBusConnection::sessionBus().connect("", "", "com.jolla.jollastore", "packageStatusChanged", this, SLOT(onPackageStatusChanged(QString, int)));
}

void ViewHelper::closeOverlay()
{
    if (overlayView) {
        QDBusConnection::sessionBus().unregisterObject("/harbour/batteryoverlay/overlay");
        QDBusConnection::sessionBus().unregisterService("harbour.batteryoverlay.overlay");
        overlayView->close();
        delete overlayView;
        overlayView = NULL;
    }
    else {
        QDBusInterface iface("harbour.batteryoverlay.overlay", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay");
        iface.call(QDBus::NoBlock, "exit");
    }
}

void ViewHelper::checkOverlay()
{
    QDBusInterface iface("harbour.batteryoverlay.overlay", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay");
    iface.call(QDBus::NoBlock, "pingOverlay");
}

void ViewHelper::startOverlay()
{
    checkActiveOverlay();
}

void ViewHelper::openStore()
{
    QDBusInterface iface("com.jolla.jollastore", "/StoreClient", "com.jolla.jollastore");
    iface.call(QDBus::NoBlock, "showApp", "harbour-batteryoverlay");
}

void ViewHelper::checkActiveSettings()
{
    bool newSettings = QDBusConnection::sessionBus().registerService("harbour.batteryoverlay.settings");
    if (newSettings) {
        showSettings();
    }
    else {
        QDBusInterface iface("harbour.batteryoverlay.settings", "/harbour/batteryoverlay/settings", "harbour.batteryoverlay");
        iface.call(QDBus::NoBlock, "show");
        qGuiApp->exit(0);
        return;
    }
}

void ViewHelper::checkActiveOverlay()
{
    bool inactive = QDBusConnection::sessionBus().registerService("harbour.batteryoverlay.overlay");
    if (inactive) {
        showOverlay();
    }
}

void ViewHelper::show()
{
    if (settingsView) {
        settingsView->raise();
        checkActiveOverlay();
    }
}

void ViewHelper::exit()
{
    QTimer::singleShot(100, qGuiApp, SLOT(quit()));
}

void ViewHelper::pingOverlay()
{
    if (overlayView) {
        Q_EMIT overlayRunning();
    }
}

void ViewHelper::showOverlay()
{
    qDebug() << "show overlay";
    QDBusConnection::sessionBus().registerObject("/harbour/batteryoverlay/overlay", this, QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);

    qGuiApp->setApplicationName("Battery Overlay");
    qGuiApp->setApplicationDisplayName("Battery Overlay");

    overlayView = SailfishApp::createView();
    overlayView->setTitle("BatteryOverlay");
    overlayView->rootContext()->setContextProperty("viewHelper", this);

    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    overlayView->setColor(color);
    overlayView->setClearBeforeRendering(true);

    overlayView->setSource(SailfishApp::pathTo("qml/overlay.qml"));
    overlayView->create();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(overlayView->handle(), QLatin1String("CATEGORY"), "notification");
    native->setWindowProperty(overlayView->handle(), QLatin1String("MOUSE_REGION"), QRegion(0, 0, 0, 0));
    overlayView->show();

    QDBusConnection::sessionBus().disconnect("", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay",
                                          "overlayRunning", this, SIGNAL(overlayRunning()));

    Q_EMIT overlayRunning();
}

void ViewHelper::showSettings()
{
    qDebug() << "show settings";
    QDBusConnection::sessionBus().registerObject("/harbour/batteryoverlay/settings", this, QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);

    qGuiApp->setApplicationName("Battery Overlay Settings");
    qGuiApp->setApplicationDisplayName("Battery Overlay Settings");

    settingsView = SailfishApp::createView();
    settingsView->setTitle("BatteryOverlay Settings");
    settingsView->rootContext()->setContextProperty("helper", this);
    settingsView->rootContext()->setContextProperty("colorHelper", new ColorHelper(this));
    settingsView->setSource(SailfishApp::pathTo("qml/settings.qml"));
    settingsView->showFullScreen();

    if (!overlayView) {
        QDBusConnection::sessionBus().connect("", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay",
                                              "overlayRunning", this, SIGNAL(overlayRunning()));
    }

    QObject::connect(settingsView, SIGNAL(destroyed()), this, SLOT(onSettingsDestroyed()));
    QObject::connect(settingsView, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(onSettingsClosing(QQuickCloseEvent*)));
}

void ViewHelper::onPackageStatusChanged(const QString &package, int status)
{
    if (package == "harbour-batteryoverlay" && status != 1) {
        Q_EMIT applicationRemoval();
    }
}

void ViewHelper::onSettingsDestroyed()
{
    QObject::disconnect(settingsView, 0, 0, 0);
    settingsView = NULL;
}

void ViewHelper::onSettingsClosing(QQuickCloseEvent *)
{
    settingsView->destroy();
    settingsView->deleteLater();

    QDBusConnection::sessionBus().unregisterObject("/harbour/batteryoverlay/settings");
    QDBusConnection::sessionBus().unregisterService("harbour.batteryoverlay.settings");

    //QDBusConnection::sessionBus().disconnect("", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay",
    //                                      "overlayRunning", 0, 0);
}
