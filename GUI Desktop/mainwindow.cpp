#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QObject>
#include <QSettings>
#include <QString>

QString code;

int checkToken(QTcpSocket *socket){
    QSettings settings("TalkBradarDev", "TalkBradar");
    QString token = settings.value("token", "").toString();
    qDebug()<<"current user token is "<<token;
    if (token.isEmpty()) {
        qDebug() << "Token Dont Exist";
        return 1;
    }
    else {
        code ='2';
        socket->write(code.toUtf8());
        socket->flush();


        socket->write(token.toUtf8());
        socket->flush();

        socket->waitForReadyRead(2000);
        QByteArray data = socket->read(1); // read 1 byte

        QString str = QString::fromUtf8(data);
        if(str=="1"){
            qDebug()<<"token match";
            return 0;
        }
        else if(str=="2") {
            qDebug()<<"token dont exist in server";
            return 1;}

    }
}







MainWindow::MainWindow(QWidget *parent, QTcpSocket *socket)
    : QMainWindow(parent)
    , ui(new Ui::TalkBrader)
    , socket(socket)
{
    QLabel *label=new QLabel("my app!");
    label->setWindowTitle("TalkBrader");

    ui->setupUi(this);
    if(checkToken(socket)==1)ui->stackedWidget->setCurrentWidget(ui->signoutPage); else ui->stackedWidget->setCurrentWidget(ui->chatPage);

}

MainWindow::~MainWindow()
{
    delete ui;
}
// go to add friend page
void MainWindow::on_addFriendButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->addFriendPage);
}

// go back to chat page
void MainWindow::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->addFriendPage);
}


void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}


void MainWindow::on_searchB_clicked()
{
    QString name = ui->searchBar->text();

    code = '5';
    socket->write(code.toUtf8());
    socket->flush();

    socket->write(name.toUtf8());
    socket->flush();
    ui->searchBar->clear();

    socket->waitForReadyRead(2000);
    QByteArray data = socket->read(25);
    if(data.isEmpty()) {
        qDebug()<<"no data or no connection";
        //ui->pushButton->setEnabled(true);
        return;
    }

    QString str = QString::fromUtf8(data);
    qDebug()<<str;

}




void MainWindow::on_loginB_clicked()
{
      ui->stackedWidget->setCurrentWidget(ui->loginPage);
}


void MainWindow::on_loginB_2_clicked()
{
    ui->pushButton->setEnabled(false);
    ui->label_11->setText("");
    ui->label_11->setStyleSheet("color: red;");

    QString username = ui->usernameBar->text();
    QString password = ui->passwordBar->text();


    if(username.size()<4){ui->label_11->setText("Username Must Be Atleast 4 Characters!");return;}
    else if(username.size()>25){ui->label_11->setText("Username Is Too Long!");return;}
    else if(password.size()<8) {ui->label_11->setText("Passwords Must Be Atleast 8 Characters!");return;}
    else if(password.size()>15){ui->label_11->setText("Password Is Too Long!");return;}



    qDebug()<<username<<" "<<password;
    code ='3';
    socket->write(code.toUtf8());
    socket->flush();

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    socket->write(data);
    socket->flush();



    socket->waitForReadyRead(2000);
    QByteArray data2 = socket->read(1); // read 1 byte
    if(data2.isEmpty()) {
        qDebug()<<"no code or no connection";
        ui->pushButton->setEnabled(true);
        return;
    }

    QString str = QString::fromUtf8(data2);

    if(str=="1"){
        ui->label_11->setText("Wrong Username Or Password!");
        ui->pushButton->setEnabled(true);
        return;
    }
    else if (str =="2"){
        ui->label_11->setText("Logging In...");
        ui->label_11->setStyleSheet("color: green;");
        qDebug()<<"log in";
        socket->waitForReadyRead(2000);
        QByteArray tokenData = socket->read(10);
        if(tokenData.isEmpty()) {
            qDebug()<<"no code or no connection";
            ui->pushButton->setEnabled(true);
            return;
        }
        QString token = QString::fromUtf8(tokenData);
        QSettings settings("TalkBradarDev", "TalkBradar");
        settings.setValue("token", token);
    }

    ui->pushButton->setEnabled(true);
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}


void MainWindow::on_registerB_2_clicked()
{
     ui->stackedWidget->setCurrentWidget(ui->registerPage);
}


void MainWindow::on_loginBRegister_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}


void MainWindow::on_registerBRegister_clicked()
{
    ui->pushButton->setEnabled(false);
     ui->label_10->setText("");
     ui->label_10->setStyleSheet("color: red;");
    QString username = ui->usernameBarRigster->text();
    QString password = ui->passwordBarRigster->text();
    QString Rpassword = ui->passwordBarRigster_2->text();


    if(username.size()<4){ui->label_10->setText("Username Must Be Atleast 4 Characters!");return;}
    else if(username.size()>25){ui->label_10->setText("Username Is Too Long!");return;}
    else if(password.size()<8) {ui->label_10->setText("Passwords Must Be Atleast 8 Characters!");return;}
    else if(password.size()>15){ui->label_10->setText("Password Is Too Long!");return;}
    else if(password != Rpassword) {ui->label_10->setText("Passwords Do Not Match!");return;}

    qDebug()<<username<<" "<<password<<" "<<Rpassword;


    code ='4';
    socket->write(code.toUtf8());
    socket->flush();

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;


    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    socket->write(data);
    socket->flush();







    socket->waitForReadyRead(2000);
    QByteArray data2 = socket->read(1); // read 1 byte
    if(data2.isEmpty()) {
        qDebug()<<"no code or no connection";
        ui->pushButton->setEnabled(true);
        return;
    }

    QString str = QString::fromUtf8(data2); qDebug() << "Received:" << str;
    if(str=="1"){
        ui->label_10->setText("Username Is Already Exist!");
        ui->pushButton->setEnabled(true);
        return;
    }
    else if(str=="2"){
        ui->label_10->setText("Creating An Account...");
        ui->label_10->setStyleSheet("color: green;");

         socket->waitForReadyRead(3000);
        QByteArray tokenData = socket->read(10); // read 1 byte
        if(tokenData.isEmpty()) {
            qDebug()<<"no data or no connection";
            ui->pushButton->setEnabled(true);
            return;
        }
        QString token = QString::fromUtf8(tokenData);
        QSettings settings("TalkBradarDev", "TalkBradar");
        settings.setValue("token", token);



    }
   qDebug() << "you are IN" << str;
   ui->pushButton->setEnabled(true);
   ui->stackedWidget->setCurrentWidget(ui->chatPage);


}


void MainWindow::on_registerB_clicked()
{
     ui->stackedWidget->setCurrentWidget(ui->registerPage);
}

