#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <QObject>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include "sailfishapp.h"
#include <mlite5/MGConfItem>
#include <QDBusInterface>
#include <QDBusConnection>

class ViewHelper : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "harbour.batteryoverlay")

public:
    explicit ViewHelper(QObject *parent = 0);

    Q_INVOKABLE void closeOverlay();
    Q_INVOKABLE void startOverlay();

public slots:
    void checkActive();

    Q_SCRIPTABLE Q_NOREPLY void show();
    Q_SCRIPTABLE Q_NOREPLY void exit();
    Q_SCRIPTABLE Q_NOREPLY void checkOverlay();

signals:
    Q_SCRIPTABLE void overlayRunning();

private:
    void showOverlay();
    void showSettings();

    QQuickView *view;
    bool m_isOverlay;

private slots:
    void onPackageStatusChanged(const QString &package, int status);

};

#endif // VIEWHELPER_H
