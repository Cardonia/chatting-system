#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jsonSendServer.h"
#include "publicVariables.h"
#include "socketManager.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QPushButton>
#include <QJsonArray>
#include <QCryptographicHash>

//constructor
MainWindow::MainWindow() : QMainWindow(nullptr)
    // Creates a MainWindow object with no parent window
    // Calls QMainWindow constructor with nullptr for ownership/memory management
    , ui(new Ui::TalkBrader)
// Creates the UI object from the .ui file and stores its pointer in 'ui'
{
    ui->setupUi(this);
    // Sets up all the widgets from the .ui file inside this MainWindow
    // 'this' is a pointer to the current MainWindow object

    checkToken();
}


//function to check Token
void MainWindow::checkToken(){
    QSettings settings("TalkBradarDev", "TalkBradar");
    token = settings.value("token", "").toString();
    qDebug()<<"current user token is "<<token;

    if (token.isEmpty()) {
        qDebug() << "Token Dont Exist";
        ui->stackedWidget->setCurrentWidget(ui->signoutPage);
    }
    else {
        qDebug() << "Token Exist";
        QJsonObject json;
        json["event"] = "TOKEN_CHECK";
        json["token"] = token;

        jsonSend(json);
    }
}








void MainWindow::runWhenDataReceived()
{
    // For now, just test
    qDebug() << "Data received!";

    static QByteArray buffer;
    //static keeps the data between multiple calls.
    //buffer Qt array variable name
    buffer.append(socketManager::socketPtr->readAll());
    //socket->readAll() read all data from socket
    //buffer.append() put data in the buffer
    while (buffer.size() >= 4) {
        //need at least 4 bytes to read the message length.If less than 4 bytes then wait for more data
        QDataStream stream(buffer);
        //QDataStream = allows reading binary data easily. stream is a name
        stream.setVersion(QDataStream::Qt_6_0);
        //set stream version so both side agrees
        quint32 length;
        //length is 4 byte variable
        stream >> length;
        //Read the first 4 bytes from stream and store it in length

        if (buffer.size() - 4 < length)return;
        //Checks if we have the full JSON message yet.
        //buffer.size() - 4 → remaining bytes after the length.
        //If not enough, exit and wait for more data.

        QByteArray jsonData = buffer.mid(4, length);
        //buffer.mid(4, length) → take length bytes starting from byte 4 (skip the 4-byte length).
        //So jsonData now contains only the JSON, without the length prefix

        buffer.remove(0, 4 + length);

        qDebug() << "buffer: "<< buffer;
        //qDebug() << "buffer: "<< buffer.toUtf8();

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        QJsonObject obj = doc.object();
        QString event = obj["event"].toString();


        if (event == "test")
            qDebug()<<"working  "<<event;
        else if(event == "REGISTER_USER_EXIST"){
            qDebug()<<"got event: "<<event;
            ui->label_10->setText("Username Is Already Exist!");
        }
        else if (event=="REGISTER_SUCCESS"){
            qDebug()<<"got event: "<<event;
            token = obj["token"].toString();
            QSettings settings("TalkBradarDev", "TalkBradar");
            settings.setValue("token", token);
            ui->stackedWidget->setCurrentWidget(ui->chatPage);
        }
        else if(event=="TOKEN_VALID"){
            qDebug()<<"TOKEN_VALID";
            ui->stackedWidget->setCurrentWidget(ui->chatPage);
        }
        else if(event=="TOKEN_INVALID"){
            qDebug()<<"TOKEN_INVALID";
            ui->stackedWidget->setCurrentWidget(ui->signoutPage);
        }
        else if(event=="LOGIN_FAILED"){
            qDebug()<<"LOGIN_FAILED";
            ui->label_11->setText("Wrong Username Or Password!");
        }

        else if(event=="LOGIN_SUCCESS"){
            qDebug()<<"LOGIN_SUCCESS";
            qDebug()<<"log in";
            token = obj["token"].toString();
            QSettings settings("TalkBradarDev", "TalkBradar");
            settings.setValue("token", token);
            ui->stackedWidget->setCurrentWidget(ui->chatPage);
        }
        else if(event=="SEARCH_FRIEND_RESULT"){
            qDebug()<<"SEARCH_FRIEND_RESULT";

            //example
            //names{sam}
            //names id{1]

            QJsonArray names = obj["names"].toArray();
            //convert the json arrays to normal arrays

            QStringList nameList;
            //QStringList is modified vector. has more functions.

            for (const QJsonValue &val : names) {
                nameList.append(val.toString());
            }
            //loop to put names into the vector
            //&val hold reference of each value in the names then put in the vector. &val = no copying
            //.append is push_back()

            int count = names.size();


            //example
            //namelist { sam }

            QJsonArray names_id = obj["names_id"].toArray();
            //convert the json arrays to normal arrays

            QStringList name_id_List;
            //QStringList is modified vector. has more functions.

            for (const QJsonValue &val2 : names_id) {
                name_id_List.append(QString::number(val2.toDouble()));
            }

            //example
            //name_id_List {1}

            QWidget *container = new QWidget;               // new container for buttons
            QVBoxLayout *layout = new QVBoxLayout(container);

            // Create buttons dynamically
            for (int i = 0; i < count; ++i) {
                QPushButton *b = new QPushButton(nameList[i]);
                layout->addWidget(b);

                connect(b, &QPushButton::clicked, this, [b,name_id_List,nameList]() {
                    QString toId;
                    for(int i = 0; i<nameList.size(); i++){
                        if(b->text()==nameList[i]) toId = name_id_List[i];
                    }


                    QJsonObject json;
                    json["event"]  = "SEND_FRIEND_REQUEST";
                    json["token"]  = token;
                    json["toId"] = toId.toInt();
                    qDebug() << "send friend request to: " << b->text()<< "Its ID "<<toId.toInt();

                    jsonSend(json);
                });

            }
            layout->addStretch();  // pushes buttons to top

            container->setLayout(layout);

            // Set the container to the scroll area from UI
            ui->scrollArea->setWidget(container);
            ui->scrollArea->setWidgetResizable(true);

        }
        else if(event=="FRIEND_REQUEST_PENDING_LIST"){
            qDebug()<<"FRIEND_REQUEST_PENDING_LIST";

            QJsonArray names = obj["names"].toArray();
            //convert the json arrays to normal arrays

            QStringList nameList;
            //QStringList is modified vector. has more functions.

            for (const QJsonValue &val : names) {
                nameList.append(val.toString());
            }


            int count = names.size();


            //example
            //namelist { sam }

            QJsonArray names_id = obj["names_id"].toArray();
            //convert the json arrays to normal arrays

            QStringList name_id_List;
            //QStringList is modified vector. has more functions.

            for (const QJsonValue &val2 : names_id) {
                name_id_List.append(QString::number(val2.toDouble()));
            }




            QWidget *container = new QWidget;               // new container for buttons
            QVBoxLayout *layout = new QVBoxLayout(container);

            // Create buttons dynamically
            for (int i = 0; i < count; ++i) {
                QPushButton *b = new QPushButton(nameList[i]);
                layout->addWidget(b);

                connect(b, &QPushButton::clicked, this, [b,name_id_List,nameList]() {  // <-- capture 'this' too
                    QString toId;
                    for(int i = 0; i<nameList.size(); i++){
                        if(b->text()==nameList[i]) toId = name_id_List[i];
                    }
                    QJsonObject json;
                    json["event"]  = "ACCEPT_FRIEND_REQUEST";
                    json["token"]   = token;
                    json["toId"] = toId.toInt();
                    qDebug() << "accept friend request to: " << toId.toInt()<<" id";

                    jsonSend(json);  // now works because 'this' is captured


                });

            }
            layout->addStretch();  // pushes buttons to top

            container->setLayout(layout);

            // Set the container to the scroll area from UI
            ui->scrollArea_2->setWidget(container);
            ui->scrollArea_2->setWidgetResizable(true);




        }

    }
}





void MainWindow::on_searchB_clicked()
{
    QString name = ui->searchBar->text();
    if(name.isEmpty()) return;
    ui->pushButton->setEnabled(false);

    QJsonObject json;
    json["event"] = "SEARCH_FRIEND";
    json["name"] = name;

    jsonSend(json);

    ui->searchBar->clear();
    qDebug() << "sent " << name;

    ui->pushButton->setEnabled(true);
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

    QJsonObject json;
    json["event"] = "LOGIN";
    json["username"] = username;

    json["password"] = password;

    jsonSend(json);
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


    QJsonObject json;
    json["event"] = "REGISTER";
    json["username"] = username;
    json["password"] = password;

    jsonSend(json);

    ui->pushButton->setEnabled(true);
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

void MainWindow::on_loginB_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

void MainWindow::on_registerB_2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->registerPage);
}

void MainWindow::on_loginBRegister_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

void MainWindow::on_registerB_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->registerPage);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->RequetPage);
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}


