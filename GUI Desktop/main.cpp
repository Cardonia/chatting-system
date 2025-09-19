#include "mainwindow.h"

#include <QTcpSocket>
#include <QObject>
#include <QApplication>
#include <QSettings>
#include <QString>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("127.0.0.1", 5000);

    if(socket->waitForConnected(3000)) { // wait max 3 seconds
        qDebug() << "Connected to server!";
    } else {
        qDebug() << "Failed to connect: " << socket->errorString();
    }


     MainWindow w(nullptr, socket);
    w.show();





    return a.exec();
}
