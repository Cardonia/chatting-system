#ifndef GETDATAFROMSERVER_H
#define GETDATAFROMSERVER_H

#include <QObject>
#include <QTcpSocket>
class MainWindow;
class getDataFromServer : public QObject {
    // Inherit from QObject to use signals, slots, and Qt’s meta-object features
    Q_OBJECT
    //This is a Qt macro.
    //It enables signals and slots, Qt’s special event system.
public:

    getDataFromServer(MainWindow* w = nullptr) : mainWindow(w) {}
    // Constructor - Called when creating a getDataFromServer object
    // w = nullptr means: if no MainWindow is provided, use nullptr (empty/nothing)
    // : mainWindow(w) is an "initializer list" - sets mainWindow to w immediately
    // This connects the data handler to a specific window for processing data


    void runWhenDataArrive();


private:
    MainWindow* mainWindow;
    //make mainWindow pointer from the only one object of MainWindow


};
#endif // GETDATAFROMSERVER_H
