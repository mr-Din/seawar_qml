#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "GameModel.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<GameModel>("GameLogicModule", 1, 0, "GameLogic");

    GameModel gameModel;
    engine.rootContext()->setContextProperty("gameLogic", &gameModel);

    // Загрузка главного меню
    const QUrl url(u"qrc:/seawar/MainMenu.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

//    const QUrl url(u"qrc:/seawar/Main.qml"_qs);
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
//        &app, []() { QCoreApplication::exit(-1); },
//        Qt::QueuedConnection);
//    engine.load(url);

    return app.exec();
}
