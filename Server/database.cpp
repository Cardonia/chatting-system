#include "database.h"
#include "functions.h"
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

Database& Database::getInstance(const std::string& path) {
	//Database& =reference to a Database object.
	//Database::getInstance mean getInstance function is from Database class
	static Database instance(path);
	//static live forever, Database call for constructor, instance is a temprorary name.
	return instance;
}

Database::Database(const std::string& path) : dbPath(path), db(nullptr) {
	openDatabase();
	initializeDatabase();
}

//this runs when the program ends or closed
Database::~Database() {
	closeDatabase();
}



//try to open database if not opened return false else return true
bool Database::openDatabase() {
	int dbCode = sqlite3_open(dbPath.c_str(), &db);	
	//If dbCode == SQLITE_OK(value 0) -> success.
	//If dbCode != SQLITE_OK -> some error happened.
	if (dbCode != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	return true;
}


void Database::closeDatabase() {
	if (db) {
		//db is a pointer. if its pointing to something then its true
		sqlite3_close(db);

		db = nullptr;
		//After closing, the pointer still has the old memory address.Setting it to nullptr makes it clear : �no database is open now.�
	}
}



void Database::initializeDatabase() {
	const char* sql =
		"CREATE TABLE IF NOT EXISTS users ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"username TEXT UNIQUE, "
		"password TEXT NOT NULL, "
		"token TEXT"
		");";

	//call execute function to run the sql command
	execute(sql);


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



bool Database::execute(const std::string& sql) {
	char* errMsg = nullptr; //errMsg is a pointer to an empty char
	int dbCode = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
	if (dbCode != SQLITE_OK) {
		std::cerr << "SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}
	return true;
}



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



bool Database::validateToken(const std::string& token) {
	std::string sql = "SELECT 1 FROM users WHERE token = ?;";
	sqlite3_stmt* stmt;

	bool valid = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, token.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			valid = true;
		}
		sqlite3_finalize(stmt);
	}
	return valid;

}







///////////////////////////////////////////////////////







bool Database::userExists(const std::string& username) {
	std::string sql = "SELECT 1 FROM users WHERE username = ?;";
	sqlite3_stmt* stmt;

	bool exists = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			exists = true;
		}
		sqlite3_finalize(stmt);
	}
	return exists;
}






///////////////////////////////////////////////////////





bool Database::validateLogin(const std::string& username, const std::string& passwordHash) {
	std::string sql = "SELECT 1 FROM users WHERE username = ? AND password = ?;";
	sqlite3_stmt* stmt;

	bool valid = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			valid = true;
		}
		sqlite3_finalize(stmt);
	}
	return valid;
}





///////////////////////////////////////////////////////





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






///////////////////////////////////////////////////////





bool Database::registerUser(const std::string& username, const std::string& passwordHash, const std::string& token) {
	std::string sql = "INSERT INTO users (username, password, token) VALUES (?, ?, ?);";
	sqlite3_stmt* stmt;

	bool success = false;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, token.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			success = true;
		}
		sqlite3_finalize(stmt);
	}
	return success;
}



///////////////////////////////////////////////////////




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
			if(id==senderTableID)continue;
			const char* username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			//char pointer , points to char in a text in memory 
			//sqlite3_column_text(stmt, 1) return unsign char (positive 0-255) 
			//reinterpret_cast<const char*> force to change type , tell cpp treat this as text 
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




///////////////////////////////////////////////////////




void Database::addFriendRequestTable(int friendId, const std::string& hashtoken) {
	
	int senderId = whatUserIdIAM(hashtoken);
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
			return;
		}
		else {
			std::cout << "request friendship don't exist from past" << std::endl;
			
		}
		sqlite3_finalize(stmt);
	}


	std::string sqlInsert = "INSERT INTO friendships (user_id, friend_id, status) VALUES (?, ?, 'pending');";
	if (sqlite3_prepare_v2(db, sqlInsert.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, senderId);
		sqlite3_bind_int(stmt, 2, friendId);

		
		if (sqlite3_step(stmt) == SQLITE_DONE) {
			std::cout << "Friend request sent from " << senderId << " to " << friendId << std::endl;
		}
		else {
			std::cout << "Failed to send friend request!" << std::endl;
		}
		sqlite3_finalize(stmt);
	}
	//send update for the reciver friend request if online
	auto it = onlineClients.find(hashtoken);
	//return iterator(like pointer thing!!) if the hash exist in map
	//iterator points to the element if found, else it point to last fake element by default
	if (it != onlineClients.end()) {
		//checks if the hash was found, if it wanst last fake element then it found it
		int fd = it->second;
		updateAllClientData(fd, hashtoken);
	}
}





////////////////////////////////////////////////////////////////////////////////////////////////





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
				const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
				//char pointer , points to char in a text in memory 
				//sqlite3_column_text(stmt, 1) return unsign char (positive 0-255) 
				//reinterpret_cast<const char*> force to change type , tell cpp treat this as text 
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





/////////////////////////////////////////////////////////////////////////////////////////////////






void Database::acceptFriendRequest(const int& toFriendId, const std::string& hashToken) {
	
	
	int senderId = whatUserIdIAM(hashToken);
	if(senderId == -1){
		std::cout<<"Debug: Faild userID for acceptFriendRequest: "<<senderId<<'\n';
		return;
	}
	sqlite3_stmt* stmt;

	std::string sql = 
	"UPDATE friendships SET status = 'accepted' WHERE "
	"user_id = ? AND friend_id = ? AND status = 'pending';";

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, toFriendId);
		sqlite3_bind_int(stmt, 2, senderId);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			std::cout << "Friend request accepted from " << senderId << " to " << toFriendId << std::endl;
		}
		else {
			std::cout << "Failed to accept friend request!" << std::endl;
		}
		sqlite3_finalize(stmt);
	}
	else{
		std::cout << "Failed to accept friend request! prepare_v2 failed" << std::endl;
		return;
	}
}







int Database::whatUserIdIAM(const std::string& hashToken) {
	sqlite3_stmt* stmt;
	std::string sql = "SELECT id FROM users WHERE token = ?;";
	int clientId = -1;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, hashToken.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			clientId = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
	}
	else std::cout << "Invalid token, id not found!" << std::endl;
		
	return  clientId;
}







json Database::getAllFriendsListFromTable(const int& clientId){
	std::vector<std::string> names;
	std::vector<int> names_id;

	std::string sql = "SELECT user_id FROM friendships WHERE friend_id = ? AND status = 'accepted';";
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
				const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
				//char pointer , points to char in a text in memory 
				//sqlite3_column_text(stmt, 1) return unsign char (positive 0-255) 
				//reinterpret_cast<const char*> force to change type , tell cpp treat this as text 
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



///////////////////////////////////////////////////////////////////////////////////

bool Database::checkIfTheyFriend(int& clientId , int& toFriendId){

	sqlite3_stmt* stmt;

	std::string sqlCheckForExisting = 
	"SELECT 1 FROM friendships WHERE "
	"(clientId = ? AND toFriendId = ?) "
   	"OR " 
	"(clientId = ? AND toFriendId = ?);";

	if (sqlite3_prepare_v2(db, sqlCheckForExisting.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, clientId);
		sqlite3_bind_int(stmt, 2, toFriendId);
		sqlite3_bind_int(stmt, 3, toFriendId);
		sqlite3_bind_int(stmt, 4, clientId);
		
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			return true;
		}
		else {
			return false;
		}
		sqlite3_finalize(stmt);
	}
	return false;
} 



/////////////////////////////////////////////////////////






void Database::storeChatMessage(const int* fromID,const int* toID,const std::String* text){
	std::string sql = "INSERT INTO messages (from_user_id , to_user_id , text) VALUES (?, ?, ?);";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, fromID.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, toID.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			std::cout<<"message saved"<<'\n';
		}
		sqlite3_finalize(stmt);
	}
}




/////////////////////////////////////////////////////////////




std::string Database::getTokenFromID(const int& userID){

	std::string sql = "SELECT token FROM users WHERE id = ?;";
	sqlite3_stmt* stmt = nullptr;
    std::string token = "";

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, userID);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			const unsigned char* temp = sqlite3_column_text(stmt, 0);
            if(temp) token = reinterpret_cast<const char*>(temp);
		}
	}
	if(stmt) sqlite3_finalize(stmt);
	return token;
}


