#pragma once

#include "sqlite3.h"
#include <vector>
#include <string>
#include "json.hpp"
using json = nlohmann::json;

struct Message{
    bool fromMe;
    std::string text;
};

class Database {
	private:
		sqlite3* db;//db is a pointer to sqlite3 object. it will hold the database connection.
		std::string dbPath; // Path to the SQLite database file
		
		
	public:
		Database(const std::string& path); //put path of database in this constructor
		void createDatabaseTables();
		void execute(const std::string& sql);

		bool validateToken(const std::string& token);
		bool userExists(const std::string& username);
		bool validateLogin(const std::string& username, const std::string& passwordHash);
		void updateUserToken(const std::string& username, const std::string& token);
		bool registerUser(const std::string& username, const std::string& passwordHash, const std::string& token);
		void addFriendRequestTable(int friendId, const std::string& token);
		int whatUserIdIAM(const std::string& hashToken);
		json searchUsers(const std::string& name,const std::string& hash);
		json friendPendingRequest(const int& clientId);
		void acceptFriendRequest(const int& toFriendId, const std::string& hashtoken);
		json getAllFriendsListFromTable(const int& clientId);
		bool checkIfTheyFriend(int& clientId , int& toFriendId);
		void storeChatMessage(int fromID,int toID,std::string text);
		std::string getTokenFromID(const int& userID);
		std::vector<Message> getChatHistory(int myId, int friendId);
};