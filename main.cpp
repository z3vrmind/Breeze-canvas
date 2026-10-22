#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "CanvasBackend.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("KDECommunity");
    app.setApplicationName("BreezeCanvas");

    QQuickStyle::setStyle("Basic");

    qmlRegisterType<CanvasBackend>("BreezeCanvas.Backend", 1, 0, "CanvasBackend");
    qmlRegisterType<BreezeIcon>("BreezeCanvas.Backend", 1, 0, "BreezeIcon");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/BreezeCanvas/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}
