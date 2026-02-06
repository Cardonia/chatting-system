#include "chatroom.h"

ChatRoom::ChatRoom(int id, const QString &name)
    : friendId(id), friendName(name)
{
}

void ChatRoom::addMessage(const Message &msg)
{
    messages.push_back(msg);
}
