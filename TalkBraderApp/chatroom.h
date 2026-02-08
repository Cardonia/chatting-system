#ifndef CHATROOM_H
#define CHATROOM_H

#include <QString>
#include <QVector>

struct Message {
    bool fromMe;
    QString text;
};

class ChatRoom{

public:
    ChatRoom() = default;
    ChatRoom(int id, const QString &name);

    int friendId;
    QString friendName;
    QVector<Message> messages;

    void addMessage(const Message &msg);
};

#endif
