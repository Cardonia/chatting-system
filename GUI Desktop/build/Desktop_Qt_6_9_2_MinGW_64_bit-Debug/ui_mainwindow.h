/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QWidget *chatPage;
    QLabel *label;
    QPushButton *pushButton_2;
    QWidget *addFriendPage;
    QFormLayout *formLayout;
    QPushButton *pushButton;
    QLineEdit *searchBar;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout;
    QPushButton *searchB;
    QWidget *registerPage;
    QLineEdit *usernameBarRigster;
    QLineEdit *passwordBarRigster;
    QLabel *label_5;
    QLabel *label_7;
    QLabel *label_8;
    QPushButton *registerBRegister;
    QPushButton *loginBRegister;
    QLineEdit *passwordBarRigster_2;
    QLabel *label_9;
    QLabel *label_10;
    QWidget *signoutPage;
    QLabel *label_2;
    QPushButton *registerB;
    QPushButton *loginB;
    QWidget *loginPage;
    QLineEdit *usernameBar;
    QLineEdit *passwordBar;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *loginB_2;
    QPushButton *registerB_2;
    QLabel *label_6;
    QLabel *label_11;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(715, 469);
        MainWindow->setMinimumSize(QSize(554, 0));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy);
        chatPage = new QWidget();
        chatPage->setObjectName("chatPage");
        label = new QLabel(chatPage);
        label->setObjectName("label");
        label->setEnabled(true);
        label->setGeometry(QRect(160, 30, 261, 41));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setBaseSize(QSize(0, 0));
        label->setInputMethodHints(Qt::InputMethodHint::ImhNone);
        pushButton_2 = new QPushButton(chatPage);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(479, 40, 91, 29));
        stackedWidget->addWidget(chatPage);
        addFriendPage = new QWidget();
        addFriendPage->setObjectName("addFriendPage");
        formLayout = new QFormLayout(addFriendPage);
        formLayout->setObjectName("formLayout");
        pushButton = new QPushButton(addFriendPage);
        pushButton->setObjectName("pushButton");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, pushButton);

        searchBar = new QLineEdit(addFriendPage);
        searchBar->setObjectName("searchBar");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, searchBar);

        scrollArea = new QScrollArea(addFriendPage);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setMouseTracking(false);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
        scrollArea->setWidgetResizable(false);
        scrollArea->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 582, 231));
        verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout->setObjectName("verticalLayout");
        scrollArea->setWidget(scrollAreaWidgetContents);

        formLayout->setWidget(2, QFormLayout::ItemRole::SpanningRole, scrollArea);

        searchB = new QPushButton(addFriendPage);
        searchB->setObjectName("searchB");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, searchB);

        stackedWidget->addWidget(addFriendPage);
        registerPage = new QWidget();
        registerPage->setObjectName("registerPage");
        usernameBarRigster = new QLineEdit(registerPage);
        usernameBarRigster->setObjectName("usernameBarRigster");
        usernameBarRigster->setGeometry(QRect(210, 110, 201, 28));
        passwordBarRigster = new QLineEdit(registerPage);
        passwordBarRigster->setObjectName("passwordBarRigster");
        passwordBarRigster->setGeometry(QRect(210, 160, 201, 28));
        passwordBarRigster->setEchoMode(QLineEdit::EchoMode::Password);
        label_5 = new QLabel(registerPage);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(120, 160, 101, 20));
        QFont font;
        font.setPointSize(10);
        label_5->setFont(font);
        label_7 = new QLabel(registerPage);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(100, 410, 231, 20));
        label_7->setFont(font);
        label_8 = new QLabel(registerPage);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(120, 110, 101, 20));
        label_8->setFont(font);
        registerBRegister = new QPushButton(registerPage);
        registerBRegister->setObjectName("registerBRegister");
        registerBRegister->setGeometry(QRect(220, 260, 121, 61));
        loginBRegister = new QPushButton(registerPage);
        loginBRegister->setObjectName("loginBRegister");
        loginBRegister->setGeometry(QRect(310, 400, 111, 41));
        passwordBarRigster_2 = new QLineEdit(registerPage);
        passwordBarRigster_2->setObjectName("passwordBarRigster_2");
        passwordBarRigster_2->setGeometry(QRect(210, 210, 201, 28));
        passwordBarRigster_2->setEchoMode(QLineEdit::EchoMode::Password);
        label_9 = new QLabel(registerPage);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(100, 210, 101, 20));
        label_9->setFont(font);
        label_10 = new QLabel(registerPage);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(80, 60, 441, 20));
        label_10->setMouseTracking(false);
        label_10->setAutoFillBackground(false);
        label_10->setAlignment(Qt::AlignmentFlag::AlignCenter);
        stackedWidget->addWidget(registerPage);
        signoutPage = new QWidget();
        signoutPage->setObjectName("signoutPage");
        label_2 = new QLabel(signoutPage);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(60, 140, 511, 61));
        QFont font1;
        font1.setPointSize(12);
        label_2->setFont(font1);
        label_2->setAutoFillBackground(false);
        label_2->setTextFormat(Qt::TextFormat::AutoText);
        label_2->setScaledContents(false);
        registerB = new QPushButton(signoutPage);
        registerB->setObjectName("registerB");
        registerB->setGeometry(QRect(210, 300, 131, 61));
        loginB = new QPushButton(signoutPage);
        loginB->setObjectName("loginB");
        loginB->setGeometry(QRect(210, 220, 131, 61));
        stackedWidget->addWidget(signoutPage);
        loginPage = new QWidget();
        loginPage->setObjectName("loginPage");
        usernameBar = new QLineEdit(loginPage);
        usernameBar->setObjectName("usernameBar");
        usernameBar->setGeometry(QRect(210, 150, 201, 28));
        passwordBar = new QLineEdit(loginPage);
        passwordBar->setObjectName("passwordBar");
        passwordBar->setGeometry(QRect(210, 200, 201, 28));
        passwordBar->setEchoMode(QLineEdit::EchoMode::Password);
        label_3 = new QLabel(loginPage);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(120, 150, 101, 20));
        label_3->setFont(font);
        label_4 = new QLabel(loginPage);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(120, 200, 101, 20));
        label_4->setFont(font);
        loginB_2 = new QPushButton(loginPage);
        loginB_2->setObjectName("loginB_2");
        loginB_2->setGeometry(QRect(220, 260, 121, 61));
        registerB_2 = new QPushButton(loginPage);
        registerB_2->setObjectName("registerB_2");
        registerB_2->setGeometry(QRect(310, 400, 111, 41));
        label_6 = new QLabel(loginPage);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(110, 410, 231, 20));
        label_6->setFont(font);
        label_11 = new QLabel(loginPage);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(80, 60, 441, 20));
        label_11->setMouseTracking(false);
        label_11->setAutoFillBackground(false);
        label_11->setAlignment(Qt::AlignmentFlag::AlignCenter);
        stackedWidget->addWidget(loginPage);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "No Chat Avilable. Click + Icon To Chat", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "back", nullptr));
        searchB->setText(QCoreApplication::translate("MainWindow", "search", nullptr));
        passwordBarRigster->setText(QString());
        label_5->setText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Already Have An Account?", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Username", nullptr));
        registerBRegister->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        loginBRegister->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        passwordBarRigster_2->setText(QString());
        label_9->setText(QCoreApplication::translate("MainWindow", "Re-Password", nullptr));
        label_10->setText(QString());
        label_2->setText(QCoreApplication::translate("MainWindow", "Log in to an existing account or create a new one.", nullptr));
        registerB->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        loginB->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        passwordBar->setText(QString());
        label_3->setText(QCoreApplication::translate("MainWindow", "Username", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        loginB_2->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        registerB_2->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Dont Have An Account?", nullptr));
        label_11->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
