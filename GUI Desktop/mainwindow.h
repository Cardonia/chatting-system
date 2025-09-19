#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class TalkBrader;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QTcpSocket *socket = nullptr);
    ~MainWindow();

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

private:
    Ui::TalkBrader *ui;
    QTcpSocket *m_socket;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
