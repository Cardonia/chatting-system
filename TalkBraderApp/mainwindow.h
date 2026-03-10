#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QString>


QT_BEGIN_NAMESPACE
namespace Ui {
class TalkBrader;
}
QT_END_NAMESPACE
// Forward-declare the UI class generated from the .ui file inside the Ui namespace


class MainWindow : public QMainWindow
//public QMainWindow means get all public functions of QMainWindow to my MainWindow class now my class has all publics of QMainWindow class
{
    Q_OBJECT // Enables signals, slots, and Qt’s meta-object system

public:


    MainWindow();
    //default constructor.



    void checkToken();

public slots:
    void runWhenDataReceived(); // the function called when data arrives

private slots:
    void on_addFriendButton_clicked();
    void on_backButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_searchB_clicked();

    void on_pushButton_4_clicked();

    void on_loginB_clicked();

    void on_loginB_2_clicked();

    void on_registerB_2_clicked();

    void on_loginBRegister_clicked();

    void on_registerBRegister_clicked();

    void on_registerB_clicked();

    void on_pushButton_3_clicked();

    void on_chatRoom_backButton_clicked();

    void on_chatRoom_sendButton_clicked();

    void updateChatRoom();
    void addOneChatMessage(const int &id , const QString text);

private:
    Ui::TalkBrader *ui;
    // Pointer to the UI elements created from the .ui file


    //TalkBrader *ui;
    //QTcpSocket *m_socket;
    //QTcpSocket *socket;
};
#endif // MAINWINDOW_H
