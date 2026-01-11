#ifndef NOCONNECTION_H
#define NOCONNECTION_H


#include <QWidget>
//main class for window

QT_BEGIN_NAMESPACE
namespace Ui {
class noconnection;
}
QT_END_NAMESPACE

class NoConnection : public QWidget{
    //create a class named noConnection and get all fucntions from QWidget class. (all the QWidget's public functions)

    Q_OBJECT
    //Q_OBJECT is a macro (special instruction) that must appear inside any class that uses Qt’s meta-object system.
    //It enables features like signals, slots, and dynamic properties
    //Signals Used to notify other objects that something happened.
    //Slots Functions that can be called automatically when a signal is emitted.

public:
    //public constructor
    explicit NoConnection(QWidget *parent = nullptr);
    //NoConnection the constructor
    //QWidget *parent -> This is a pointer to another QWidget, usually the (parent window).
    //nullptr It gives a default value for the parameter.
    //explicit keyword prevents implicit type conversions.


private:
    Ui::noconnection *ui;


};

#endif // NOCONNECTION_H
