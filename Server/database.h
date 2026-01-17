#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <vector>
#include <string>
#include <mutex>

#include "json.hpp"
using json = nlohmann::json;

class Database {
	private:
		sqlite3* db;
		//db is a pointer to sqlite3 object. it will hold the database connection.

		std::string dbPath; // Path to the SQLite database file
		

		std::mutex dbMutex;

		// Private constructor for singleton pattern(Only one object of a class can exist in the whole program.)
		Database(const std::string& path); //put path of database in this constructor
	    //its a constructor so database name is the class name. its been just declared here. the definition is in database.cpp
		
		//PRIVATE functions
		bool openDatabase();
		void closeDatabase();

	public:
		// Singleton instance getter (The word �instance� just means an object of a class)
		static Database& getInstance(const std::string& path = "users.db");
		//static Means you can call this function without creating a Database object.
		//& no copy of object is made. just address is given.
		//(= "users.db" ) -> default value,if don�t pass anything, it uses "users.db"

		Database(const Database&) = delete; // Delete copy constructor
		Database& operator=(const Database&) = delete; // Delete copy assignment operator

		~Database(); // Destructor to close the database connection


		// User operations
		void showTableOnConsole();
		bool validateToken(const std::string& token);
		bool userExists(const std::string& username);
		bool validateLogin(const std::string& username, const std::string& passwordHash);
		void updateUserToken(const std::string& username, const std::string& token);
		bool registerUser(const std::string& username, const std::string& passwordHash, const std::string& token);
		void addFriendRequestTable(int friendId, const std::string& token);
		int whatUserIdIAM(const std::string hashToken);
		json searchUsers(const std::string& query);
		std::vector<std::string> friendPendingRequest(const int clientId);
		void acceptFriendRequest(const std::string& friendName, const std::string& hashtoken);



		//utilaty
		void initializeDatabase();
		bool execute(const std::string& sql);
		

};



#endif 
