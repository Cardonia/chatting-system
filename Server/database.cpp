#include "database.h"
#include "functions.h"
#include <iostream>




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
		//After closing, the pointer still has the old memory address.Setting it to nullptr makes it clear : “no database is open now.”
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


void Database::showTableOnConsole() {
	const char* sql1 = "SELECT * FROM users;";

	// Inline callback as lambda
	auto callback = [](void* NotUsed, int argc, char** argv, char** azColName) -> int {
		for (int i = 0; i < argc; i++) {
			std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
		}
		std::cout << std::endl;
		return 0;
		};

	sqlite3_exec(db, sql1, callback, nullptr, nullptr);


}



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

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			success = true;
		}
		sqlite3_finalize(stmt);
	}
	return success;
}






std::vector<std::string> Database::searchUsers(const std::string& name) {
	std::vector<std::string> results;
	std::string sql = "SELECT username FROM users WHERE username LIKE ? LIMIT 15;";
	sqlite3_stmt* stmt;

	std::string searchPattern = name + "%";

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			const char* username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
			if (username) {
				results.push_back(username);
			}
		}
		sqlite3_finalize(stmt);
	}
	return results;
}








void Database::addFriendRequestTable(const std::string& friendName, const std::string& hashtoken) {
	
	sqlite3_stmt* stmt;
	

	std::string sqlSender = "SELECT id FROM users WHERE token = ?;";
	int senderId = -1;
	if (sqlite3_prepare_v2(db, sqlSender.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, hashtoken.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			senderId = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
	}
	//get sender id from token in users table

	if (senderId == -1) {std::cout << "Invalid token, sender not found!" << std::endl;return;}

	
	std::string sqlFriend = "SELECT id FROM users WHERE username = ?;";
	int friendId = -1;
	if (sqlite3_prepare_v2(db, sqlFriend.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, friendName.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			friendId = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
	}
	//get friend id from username in users table

	if (friendId == -1) {std::cout << "Friend username not found!" << std::endl;return;}

	
	

	std::string sqlCheckForExisting = "SELECT 1 FROM friendships WHERE user_id = ? AND friend_id = ?;";
	if (sqlite3_prepare_v2(db, sqlCheckForExisting.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, senderId);
		sqlite3_bind_int(stmt, 2, friendId);


		if (sqlite3_step(stmt) == SQLITE_ROW) {
			std::cout << "request friendship exist from past" << std::endl;
			return;
		}
		else {
			std::cout << "request friendship don't exist from past" << std::endl;
			
		}
		sqlite3_finalize(stmt);
	}


	if (sqlite3_prepare_v2(db, sqlCheckForExisting.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, friendId);
		sqlite3_bind_int(stmt, 2, senderId);

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


	if (onlineClients.find(hashtoken) != onlineClients.end()){
		
		int sock = onlineClients[hashtoken];
		// You can now send data to sock
		updateAllClientData(sock, hashtoken);
	}
	
}







int Database::whatUserIdIAM(const std::string hashToken) {
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

	if (clientId == -1) {
		std::cout << "Invalid token, id not found!" << std::endl;
		return -1;
	}

	return  clientId;
}





std::vector<std::string> Database::friendPendingRequest(const int clientId) {

	
	std::vector<std::string> results;
	std::string sql = "SELECT users.username FROM friendships JOIN users ON friendships.user_id = users.id WHERE friendships.friend_id = ? AND friendships.status = 'pending';";
	sqlite3_stmt* stmt;

	int a = clientId;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, clientId);

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			const char* username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
			if (username) {
				results.push_back(username);
			}
		}
		sqlite3_finalize(stmt);
	}
	return results;
}







void Database::acceptFriendRequest(const std::string& friendName, const std::string& hashToken) {
	sqlite3_stmt* stmt;
	std::cout <<"hashToken" << hashToken << std::endl;
	// Step 1: Get your user ID from your token
	std::string sqlFriendID = "SELECT id FROM users WHERE token = ?;";
	int friendCode = -1;
	if (sqlite3_prepare_v2(db, sqlFriendID.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, hashToken.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			friendCode = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
	}

	if (friendCode == -1) {
		std::cout << "Invalid token, friendID not found!" << std::endl;
		return;
	}

	// Step 2: Get sender ID from their username
	std::string sqlSender = "SELECT id FROM users WHERE username = ?;";
	int senderId = -1;
	if (sqlite3_prepare_v2(db, sqlSender.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, friendName.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			senderId = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
	}

	if (senderId == -1) {
		std::cout << "Sender username not found!" << std::endl;
		return;
	}

	// Step 3: Update the friendship status to 'accepted'
	std::string sqlUpdate = "UPDATE friendships SET status = 'accepted' WHERE user_id = ? AND friend_id = ? AND status = 'pending';";
	if (sqlite3_prepare_v2(db, sqlUpdate.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, senderId);
		sqlite3_bind_int(stmt, 2, friendCode);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			std::cout << "Friend request accepted from " << senderId << " to " << friendCode << std::endl;
		}
		else {
			std::cout << "Failed to accept friend request!" << std::endl;
		}
		sqlite3_finalize(stmt);
	}

	
}