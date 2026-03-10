#include "database.h"
#include "functions.h"
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

//constructor 
Database::Database(const std::string& path){
	//get the database file name and set db pointer to null
	dbPath = path;
    db = nullptr;
	//open the databse path and use db pointer to point to it
	if(sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK){
		std::cout<<"Cant open database file\n";
		std::exit(1);
	}
	//create the tables if not exist
	createDatabaseTables();
}

void Database::createDatabaseTables() {
	const char* sql =
		"CREATE TABLE IF NOT EXISTS users ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"username TEXT UNIQUE, "
		"password TEXT NOT NULL, "
		"token TEXT"
		");";
	execute(sql);//call execute function to run the sql command

	const char* sql2 =
		"CREATE TABLE IF NOT EXISTS friendships("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"user_id INTEGER NOT NULL, "
		"friend_id INTEGER NOT NULL, "
		"status TEXT NOT NULL CHECK(status IN('pending', 'accepted', 'blocked')),"
		"created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
		"FOREIGN KEY(user_id) REFERENCES users(id),"
		"FOREIGN KEY(friend_id) REFERENCES users(id),"
		"UNIQUE(user_id, friend_id)"
		");";
	execute(sql2);

	const char* sql3 = 
	"CREATE TABLE IF NOT EXISTS messages ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "from_user_id INTEGER NOT NULL,"
    "to_user_id INTEGER NOT NULL,"
    "text TEXT NOT NULL,"
    "datetime DATETIME DEFAULT CURRENT_TIMESTAMP"
	");";
	execute(sql3);
}

void Database::execute(const std::string& sql) {
	if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK){
		std::cerr << "SQL execute error: creating tables" << std::endl;
		std::exit(1);
	}
}

bool Database::validateToken(const std::string& token) {
	std::string sql = "SELECT 1 FROM users WHERE token = ?;";
	sqlite3_stmt* stmt;

	bool valid = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, token.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) valid = true;
		sqlite3_finalize(stmt);
	}
	return valid;
}

bool Database::userExists(const std::string& username) {
	std::string sql = "SELECT 1 FROM users WHERE username = ?;";
	sqlite3_stmt* stmt;

	bool exists = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) exists = true;
		sqlite3_finalize(stmt);
	}
	return exists;
}

bool Database::validateLogin(const std::string& username, const std::string& passwordHash) {
	std::string sql = "SELECT 1 FROM users WHERE username = ? AND password = ?;";
	sqlite3_stmt* stmt;

	bool valid = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) valid = true;
		sqlite3_finalize(stmt);
	}
	return valid;
}

void Database::updateUserToken(const std::string& username, const std::string& token) {
	std::string sql = "UPDATE users SET token = ? WHERE username = ?;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, token.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
}

bool Database::registerUser(const std::string& username, const std::string& passwordHash, const std::string& token) {
	std::string sql = "INSERT INTO users (username, password, token) VALUES (?, ?, ?);";
	sqlite3_stmt* stmt;

	bool success = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, token.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
		sqlite3_finalize(stmt);
	}
	return success;
}

int Database::whatUserIdIAM(const std::string& hashToken) {
	sqlite3_stmt* stmt;
	std::string sql = "SELECT id FROM users WHERE token = ?;";
	int clientId = -1;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, hashToken.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) clientId = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	else std::cout << "Invalid token, id not found!" << std::endl;	
	return  clientId;
}

json Database::searchUsers(const std::string& name,const std::string& hash) {
	int senderTableID = whatUserIdIAM(hash);
	std::vector<std::string> names;
	std::vector<int> names_id;

	std::string sql = "SELECT id , username FROM users WHERE username LIKE ? LIMIT 15;";
	sqlite3_stmt* stmt;
	std::string searchPattern = name + "%";

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			int id = sqlite3_column_int(stmt, 0);
			if(id == senderTableID) continue;
			//it will return unsigned char and we use  (const char*) to cornvert to char
			const char* username = (const char*)sqlite3_column_text(stmt, 1);
			if (username) {
				names.push_back(username);
				names_id.push_back(id);	
			}
		}
		sqlite3_finalize(stmt);
	}
	json j;
	j["names"] = names;
	j["names_id"] = names_id;
	return j;
}

std::string Database::getTokenFromID(const int& userID){
	std::string sql = "SELECT token FROM users WHERE id = ?;";
	sqlite3_stmt* stmt;
    std::string token = "";

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, userID);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			const unsigned char* temp = sqlite3_column_text(stmt, 0);
            if(temp) token = (const char*)temp;
		}
	}
	sqlite3_finalize(stmt);
	return token;
}

void Database::addFriendRequestTable(int friendId, const std::string& hashtoken) {
	int senderId = whatUserIdIAM(hashtoken);
	if(senderId == friendId) return;
	if (senderId == -1) {std::cout << "Invalid token, sender not found!" << std::endl;return;}
	sqlite3_stmt* stmt;
	
	std::string sqlCheckForExisting = 
	"SELECT 1 FROM friendships WHERE "
	"(user_id = ? AND friend_id = ?) "
   	"OR " 
	"(user_id = ? AND friend_id = ?);";

	if (sqlite3_prepare_v2(db, sqlCheckForExisting.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, senderId);
		sqlite3_bind_int(stmt, 2, friendId);
		sqlite3_bind_int(stmt, 3, friendId);
		sqlite3_bind_int(stmt, 4, senderId);
		std::cout<<"Sender ID: "<<senderId<<" / Reciver ID: "<<friendId<<std::endl;

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			std::cout << "request friendship exist from past" << std::endl;
			sqlite3_finalize(stmt);
			return;
		}
		else std::cout << "request friendship don't exist from past" << std::endl;
		sqlite3_finalize(stmt);
	}

	std::string sqlInsert = "INSERT INTO friendships (user_id, friend_id, status) VALUES (?, ?, 'pending');";
	if (sqlite3_prepare_v2(db, sqlInsert.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, senderId);
		sqlite3_bind_int(stmt, 2, friendId);

		if (sqlite3_step(stmt) == SQLITE_DONE) std::cout << "Friend request sent from " << senderId << " to " << friendId << std::endl;
		else std::cout << "Failed to send friend request!" << std::endl;
		sqlite3_finalize(stmt);
	}
	std::string friendHashToken = getTokenFromID(friendId);
	//send update for the reciver friend request if online
	auto it = onlineClients.find(friendHashToken);
	//return iterator(like pointer thing!!) if the hash exist in map
	//iterator points to the element if found, else it point to last fake element by default
	if (it != onlineClients.end()) {
		//checks if the hash was found, if it wanst last fake element then it found it
		int fd = it->second;
		updateAllClientData(fd, friendHashToken);
	}
}

json Database::friendPendingRequest(const int& clientId) {
	std::vector<std::string> names;
	std::vector<int> names_id;

	std::string sql = "SELECT user_id FROM friendships WHERE friend_id = ? AND status = 'pending';";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, clientId);

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			int id = sqlite3_column_int(stmt, 0);
			names_id.push_back(id);
		}
		sqlite3_finalize(stmt);
	}

	sql = "SELECT username FROM users WHERE id = ?;";
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {

		for(int value : names_id){
			sqlite3_bind_int(stmt, 1, value);
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				const char* name = (const char*)sqlite3_column_text(stmt, 0);
				if(name) names.push_back(name);
			}
			sqlite3_reset(stmt);
		}
		sqlite3_finalize(stmt);
	}
	json j;
	j["names"] = names;
	j["names_id"] = names_id;
	return j;
}

void Database::acceptFriendRequest(const int& toFriendId, const std::string& hashToken) {
	int senderId = whatUserIdIAM(hashToken);
	if(senderId == -1){
		std::cout<<"Debug: Faild userID for acceptFriendRequest: "<<senderId<<'\n';
		return;
	}
	std::string sql = "UPDATE friendships SET status = 'accepted' WHERE user_id = ? AND friend_id = ? AND status = 'pending';";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, toFriendId);
		sqlite3_bind_int(stmt, 2, senderId);

		if (sqlite3_step(stmt) == SQLITE_DONE) std::cout << "Friend request accepted from " << senderId << " to " << toFriendId << std::endl;
		else std::cout << "Failed to accept friend request!" << std::endl;
	}
	else{
		std::cout << "Failed to accept friend request! prepare_v2 failed" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}
	sqlite3_finalize(stmt);
}

json Database::getAllFriendsListFromTable(const int& clientId){
	std::vector<std::string> names;
	std::vector<int> names_id;
 
	std::string sql = "SELECT user_id, friend_id FROM friendships WHERE (user_id = ? OR friend_id = ?) AND status = 'accepted';";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, clientId);
		sqlite3_bind_int(stmt, 2, clientId);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			int id1 = sqlite3_column_int(stmt, 0);
			int id2 = sqlite3_column_int(stmt, 1);

			int friendId;
			if (id1 == clientId) friendId = id2;
			else friendId = id1;

			names_id.push_back(friendId);
		}
		sqlite3_finalize(stmt);
	}
	
	sql = "SELECT username FROM users WHERE id = ?;";
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		for(int value : names_id){
			sqlite3_bind_int(stmt, 1, value);
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				const char* name = (const char*)sqlite3_column_text(stmt, 0);
				if(name) names.push_back(name);
			}
			sqlite3_reset(stmt);
		}
		sqlite3_finalize(stmt);
	}
	json j;
	j["names"] = names;
	j["names_id"] = names_id;
	return j;
}

bool Database::checkIfTheyFriend(int& clientId , int& toFriendId){
	sqlite3_stmt* stmt;
	std::string sqlCheckForExisting = 
	"SELECT 1 FROM friendships WHERE "
	"((user_id = ? AND friend_id = ?) "
   	"OR " 
	"(user_id = ? AND friend_id = ?)) AND status =  'accepted';";

	if (sqlite3_prepare_v2(db, sqlCheckForExisting.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, clientId);
		sqlite3_bind_int(stmt, 2, toFriendId);
		sqlite3_bind_int(stmt, 3, toFriendId);
		sqlite3_bind_int(stmt, 4, clientId);
		
		bool result = (sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
        return result;
	}
	return false;
} 

void Database::storeChatMessage(int fromID,int toID, std::string text){
	std::string sql = "INSERT INTO messages (from_user_id , to_user_id , text) VALUES (?, ?, ?);";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, fromID);
		sqlite3_bind_int(stmt, 2, toID);
		sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_DONE) std::cout<<"message saved"<<'\n';
		sqlite3_finalize(stmt);
	}
}

std::vector<Message> Database::getChatHistory(int myId, int friendId){
	std::vector<Message> msgs;
	std::string sql = 
	"SELECT from_user_id, to_user_id, text "
	"FROM messages "
	"WHERE " 
	"(from_user_id = ? AND to_user_id = ?) "
	"OR "
	"(from_user_id = ? AND to_user_id = ?) "
	"ORDER BY datetime LIMIT 50;";
	sqlite3_stmt* stmt = nullptr;

	 if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK){

        sqlite3_bind_int(stmt, 1, myId);
        sqlite3_bind_int(stmt, 2, friendId);
        sqlite3_bind_int(stmt, 3, friendId);
        sqlite3_bind_int(stmt, 4, myId);

        while(sqlite3_step(stmt) == SQLITE_ROW){
			int sender = sqlite3_column_int(stmt, 0);
            const unsigned char* txt = sqlite3_column_text(stmt, 2);

			Message m;
			if(sender == myId) m.fromMe = true;
			else m.fromMe = false;

			if(txt) m.text = (const char*)txt;
			else m.text = "";

            msgs.push_back(m);
        }
    }
    sqlite3_finalize(stmt);
    return msgs;
}