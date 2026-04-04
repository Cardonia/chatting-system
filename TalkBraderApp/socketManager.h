#pragma once
#include <QTcpSocket>

class socketManager{
public:
    static QTcpSocket* socketPtr;
    //static -> only one instance of the variable for the whole program
    //* pointer to the socket obj
};

