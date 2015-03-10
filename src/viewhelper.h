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

public slots:
    void checkActive();

    Q_SCRIPTABLE void show();
    Q_SCRIPTABLE void exit();

private:
    void showOverlay();
    void showSettings();

    QQuickView *view;

private slots:
    void onPackageStatusChanged(const QString &package, int status);

};

#endif // VIEWHELPER_H
