#include "mainwindow.h"
#include "noconnection.h"
#include "socketManager.h"
#include "getDataFromServer.h"
#include <QTcpSocket>
#include <QApplication>

int main(int argc, char *argv[]){
    QApplication App(argc, argv);//Class that manages application-wide resources and GUI event loop.

    socketManager::socketPtr = new QTcpSocket();//assign socket to the created empty socket from socketManager class
    socketManager::socketPtr->connectToHost("127.0.0.1", 5000);//assign ip + port

    if(socketManager::socketPtr->waitForConnected(3000)){ // wait max 3 seconds
        qDebug() << "Connected to server!";
        MainWindow window;
        //make 1 object througth the entire code



        getDataFromServer dataHandler(&window);
        //Create object
        //Passes the main window address to constructor so handler knows where to send data
        dataHandler.runWhenDataArrive();
        //Start listening for incoming data from server



        window.show();
        return App.exec();

    } else {
        qDebug() << "Failed to connect: " << socketManager::socketPtr->errorString();
        NoConnection NoConnectionWindow;
        NoConnectionWindow.show();
        return App.exec();
    }
}
