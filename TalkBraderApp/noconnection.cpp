#include "noconnection.h"
#include "ui_noconnection.h"

#include <QLabel>
#include <QVBoxLayout>

NoConnection::NoConnection(QWidget *parent)
    //QWidget *parent -> A pointer to a QWidget that will be the parent of this window.
    : QWidget(parent)
    //QWidget(parent) -> Calls the constructor of the base class QWidget and passes parent to it.
    //NoConnection inherits from QWidget.
    //Before the NoConnection constructor runs, the base class must be constructed first.
    //This ensures your NoConnection is fully initialized as a QWidget.

    , ui(new Ui::noconnection)


{
    ui->setupUi(this);
}



