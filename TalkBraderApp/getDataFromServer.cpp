#include "getDataFromServer.h"
#include "socketManager.h"
#include "mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QPushButton>
#include <QJsonArray>




void getDataFromServer::runWhenDataArrive() {
    connect(socketManager::socketPtr, &QTcpSocket::readyRead, [this]() {
        mainWindow->runWhenDataReceived();
        //mainWindow is a pointer and run this fucntion.
    });

    qDebug() << "Handler initialized";

}
