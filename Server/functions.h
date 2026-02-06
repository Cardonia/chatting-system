#pragma once

#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <unordered_map>


#include "json.hpp"
using json = nlohmann::json;

#include "picosha2.h"
#include "database.h" 

extern std::map<std::string, int> onlineClients;

void log(const std::string& msg);
void handleClientEvent(int client, std::string msg);
void LoginHandle(int client , std::string msg);
void RegisterHandle(int client,std::string msg);
std::string generateToken();
void checkToken(int client, std::string msg);
void searchForClient(int client , std::string msg);
void readClientMsg(int client, fd_set& master_list);
void sendClientMsg(int client, json jsonObject);
void addFriendRequest(int client, std::string msg);
void friendPendingRequestList(int client, const std::string token);
void updateAllClientData(int client, std::string token);
void acceptFriendRequest(int client, std::string msg);
void getAllFriendsList(int& client ,const std::string& tokenHash);
void handleChatRoom(int& client, const std::String& msg);
void handleChatRoomMessage(int client, const std::string& msg);
handleUserSendMessage(int& client,const std::string& msg);
#pragma once