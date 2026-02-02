#pragma once
#include <string>
#include <vector>

struct chatMessage{
    int msgSenderId;
    std::string chatMsg;
}

class ChatRoom{
    private:
    int user1;
    int user2;
    std::vector<chatMessage> messages;

    public;
    //constructor 
    ChatRoom(int a, int b);

    const std::vector<Message>& getMessages() const {
        return messages;
    }


}