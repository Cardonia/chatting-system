#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <QTcpSocket>

class socketManager{
public:
    static QTcpSocket* socketPtr;
    //static -> only one instance of the variable for the whole program
    //* pointer to the socket obj
};

#endif // SOCKETMANAGER_H
