#include <iostream>
#include "sqlite3.h"
#include <winsock2.h>
//windows library for handling networking. like making sockets
#include <ws2tcpip.h>
//more futures foe winsock2.h
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
//lock for thread
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")

#include "json.hpp"
using json = nlohmann::json;

#include "picosha2.h"  // or "picosha2.h"



struct ClientInfo {
    SOCKET socket;
    std::string name;
    std::string ip;
};


std::vector<ClientInfo> clientVector;
std::mutex clientVectorMutex;
std::unordered_map<SOCKET, SOCKET> chatPairs;
//map for storing who is chatting with who , as key value pair 
//          example
//chatPairs[clientA] = clientB;
//chatPairs[clientB] = clientA;
std::unordered_map<int, bool> availableClient;


void handleClient(SOCKET client, std::string clientIP);
void Token_01(SOCKET client);
void LoginHandle(SOCKET client);
void RegisterHandle(SOCKET client);
std::string generateToken();
void checkToken(SOCKET client);
void searchForClient(SOCKET client);
sqlite3* mydb;

int main() {
   
  

	//creating or opening database
   
    int mydbCode = sqlite3_open("users.db", &mydb);

    //If mydbCode == SQLITE_OK(value 0) -> success.
    //If mydbCode != SQLITE_OK -> some error happened.

	//if mydbCode != 0 (true) means error happened
    if (mydbCode) {std::cerr << "Can't open database: " << sqlite3_errmsg(mydb) << std::endl;   return 1;}

	//sql command to create table if not exists stored in sql variable
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE, "
        "password TEXT NOT NULL, "
        "token TEXT"
        ");";
	char* errMsg = nullptr;
    mydbCode = sqlite3_exec(mydb, sql, 0, 0, &errMsg);
    if (mydbCode != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    



    /*
    const char* resetSQL = "DELETE FROM sqlite_sequence WHERE name='users';";
    errMsg = nullptr;
    int rc = sqlite3_exec(mydb, resetSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    */


    const char* sql1 = "SELECT * FROM users;";
    

    // Inline callback as lambda
    auto callback = [](void* NotUsed, int argc, char** argv, char** azColName) -> int {
        for (int i = 0; i < argc; i++) {
            std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
        }
        std::cout << std::endl;
        return 0;
        };

    mydbCode = sqlite3_exec(mydb, sql1, callback, nullptr, &errMsg);
    if (mydbCode != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }





    
	
    //for (int i = 0; i < 20; i++)std::cout << generateToken() << std::endl;

    WSADATA wsa;
    //starting up winsock2.h
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { std::cerr << "Failed to start Winsock" << std::endl;   return 1; }
    //MAKEWORD(2,2) version 2.2  , &wsa memory address , if failed return 1

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) { std::cerr << "Socket creation failed\n" << std::endl;   WSACleanup();   return 1; }
    std::cerr << "Socket successfully created  " << server_fd << std::endl;
    //making server socket
    /*
    ID / given a number by os (example 550)
    IP type / AF_INET (IPv4)
    protocol / TCP
    local ip / not assigned
    local port / not assigned
    status / none
    Send - Receive Buffers / empty
    Remote IP - Port / none (no client connected)
     */

    sockaddr_in addressHolder{};//a structure to hold ip and port
    addressHolder.sin_family = AF_INET;//IPv4 type
    addressHolder.sin_addr.s_addr = INADDR_ANY;//any IP
    addressHolder.sin_port = htons(5000);//port
    //ip type / IPv4
    //port / 5000
    //IP / any available

    if (bind(server_fd, (sockaddr*)&addressHolder, sizeof(addressHolder)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    // bind (sockaddr*)&serverAddr, sizeof(serverAddr) with ID 550
    //&serverAddr is memory address of the serverAddr structure. bind want address not a copy
    //sockaddr* mean turn this address to generic socket address pointer. because bind just read without caring about the type like ipv4 or ipv6 or others ip. we must use generic socket address pointer
    //We need the size of the structure for bind() because the OS has to know how many bytes to read from the pointer.

    /* AFTER BINDING   server_fd socket

    ID / 550
    IP type / AF_INET (IPv4)
    protocol / TCP
    local ip / any ip
    local port / 5000

    Send - Receive Buffers / empty
    Remote IP - Port / none (no client connected)
    */

    listen(server_fd, 10);// start listening on port 5000 (server_fd), max 10 clients in queue

    //status / listening

    std::cout << "server listening on port 5000" << std::endl;

    while (true) {
        
        sockaddr_in clientAddr;
        int addrSize = sizeof(clientAddr);
        //make a structure to hold clint ip

        SOCKET client_fd = accept(server_fd, (sockaddr*)&clientAddr, &addrSize);
        //DEFAULT ONE [ SOCKET client_fd = accept(server_fd, nullptr, nullptr); ]
       //null in here means i don't care to store clint ip
       //create clint socket based on server socket.
       /*new clint socket

       ID / 552 (new number, given by OS)
       IP type / AF_INET (IPv4)
       protocol / TCP
       local IP / server’s IP
       local port / 5000   (server’s listening port)

       Send - Receive Buffers / empty
       Remote IP / client’s IP
       Remote Port / client’s port
       */

        if (client_fd == INVALID_SOCKET) { std::cerr << "Accept failed\n";   continue; }
        //start from start of while loop

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        //convert clint ip to readable text
        std::cout << " New Connected Client IP: " << clientIP << std::endl;

        //create a thread for handling connected client
        std::thread handleClientThread(handleClient, client_fd, std::string(clientIP));
        handleClientThread.detach();
        //detach() the main thread don't wait for this to finish , run freely.

        

        



    }

    WSACleanup();
    //winsock2 clean up
    sqlite3_close(mydb);
    closesocket(server_fd);
    return 0;
}

void handleClient(SOCKET client, std::string clientIP){
    while (true) {
        char buffer[3] = { 0 }; // 2 digits + '\0'

        int bytesReceived = recv(client, buffer, 2, 0); // read max 2 bytes
        if (bytesReceived <= 0) { closesocket(client); return; }

        buffer[bytesReceived] = '\0'; // make string safe
        int code = atoi(buffer);       // convert string to int
        std::cout << "Received code: " << code << " from client " << clientIP << std::endl;
        switch (code) {
        case 0: std::cout << "case 0 called\n";
            
            break;
        case 2:
            std::cout << "case 2 called\n";
            checkToken(client);
            break;


        case 3:
            std::cout << "case 3 called\n";
            LoginHandle(client);
            break;

        case 4:
            std::cout << "case 4 called\n";
            RegisterHandle(client);
            break;

        case 5:
            std::cout << "case 5 called\n";
            searchForClient(client);
            break;
       
        default: std::cout << "unknown code\n";
        }
    }

}



void LoginHandle(SOCKET client) {
    std::string fullData;
    char buffer[250];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) return;\

        buffer[bytesReceived] = '\0';
        fullData += buffer;

        
        if (fullData.back() == '}') break;
        break;
    }
    
    std::cout << "Received: " << fullData << std::endl;
	
    json j = json::parse(fullData);
    std::string username = j["username"];
    std::string password = j["password"];
    std::string hash = picosha2::hash256_hex_string(password);




      std::string sql = "SELECT 1 FROM users WHERE username=? AND password=?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(mydb, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                send(client, "2", 1, 0);
                std::cout << "Login OK\n";
                std::string token = generateToken();

                sqlite3_stmt* updateStmt;
                std::string updateSQL = "UPDATE users SET token=? WHERE username=?;";
                if (sqlite3_prepare_v2(mydb, updateSQL.c_str(), -1, &updateStmt, nullptr) == SQLITE_OK) {
                    sqlite3_bind_text(updateStmt, 1, token.c_str(), -1, SQLITE_STATIC);
                    sqlite3_bind_text(updateStmt, 2, username.c_str(), -1, SQLITE_STATIC);
                    sqlite3_step(updateStmt);
                    sqlite3_finalize(updateStmt);
                }

                send(client, token.c_str(), token.size(), 0);

            } else {
                send(client, "1", 1, 0);
                std::cout << "Wrong username or password\n";
            }
        }



    /*
    std::string hash = picosha2::hash256_hex_string(password);
    std::cout << "SHA-256 of \"" << password << "\": " << hash << std::endl;
std::string token = generateToken();
    std::string sql = "INSERT INTO users (username, password, token) VALUES ('" +
        username+ "', '" +
        hash+ "', '" +
        token+ "');";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(mydb, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Insert error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    */

}

void RegisterHandle(SOCKET client) {
    std::string fullData;
    char buffer[250];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0)  return;  // client closed or error

        buffer[bytesReceived] = '\0';
        fullData += buffer;

       
        if (fullData.back() == '}') break;
        break;
    }

    std::cout << "Received: " << fullData << std::endl;
    
    json j = json::parse(fullData);
    std::string username = j["username"];
    std::string password = j["password"];






    bool userExists = false;

    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        bool* exists = static_cast<bool*>(data);
        *exists = true; // row found
        return 0;
        };

    
    std::string sql = "SELECT * FROM users WHERE username='" + username + "';";

    char* errMsg = 0;
    int rc = sqlite3_exec(mydb, sql.c_str(), callback, &userExists, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
	
    if (userExists) {
        std::cout << "Username exists" << std::endl;
        send(client, "1", 1, 0);
        return;
    }
    else send(client, "2", 1, 0);

	std::string token = generateToken();
    send(client, token.c_str(), token.size(), 0);

    

    std::string hash = picosha2::hash256_hex_string(password);
    std::cout << "SHA-256 of \"" << password << "\": " << hash << std::endl;

     sql = "INSERT INTO users (username, password, token) VALUES ('" +
        username + "', '" +
        hash + "', '" +
        token + "');";

    errMsg = nullptr;
    rc = sqlite3_exec(mydb, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Insert error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }


}

std::string generateToken() {
    srand(time(0));//for token generation
    std::string token;
	token.clear();
	token.reserve(10); // reserve space for 10 characters
    for (int i = 0; i < 10; i++) {
        char c = 49 + rand() % (122 - 49 + 1); 
        token = token + c; 
    }
	std::cout << "Generated Token: " << token << std::endl;
    return token;
}


void checkToken(SOCKET client) {

    std::string fullData;
    char buffer[15];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) return; 

            buffer[bytesReceived] = '\0';
        fullData += buffer;


        if (fullData.back() == '}') break; 
        break;
    }

    std::string tokenFromUser = fullData;


    std::string sql = "SELECT * FROM users WHERE token='" + tokenFromUser + "';";

    char* errMsg = nullptr;
    bool found = false;
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        bool* f = static_cast<bool*>(data);
        *f = true;   // row found
        return 0;
        };




    int rc = sqlite3_exec(mydb, sql.c_str(), callback, &found, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    if (found) {
        std::cout << "Token matched!" << std::endl;
        send(client, "1", 1, 0);
    }
    else {
        std::cout << "No match for token" << std::endl;
        send(client, "2", 1, 0);
    }

	

}




void searchForClient(SOCKET client) {

    //buffer size is 26. 25 for data and 1 for \0.
    char name[26];
    int bytesReceived = recv(client, name, sizeof(name) - 1, 0);
	name[bytesReceived] = '\0';
	std::cout << "Searching for: " << name << std::endl;


    std::string sql = "SELECT 1 FROM users WHERE username = ?; ";
    sqlite3_stmt* stmt;


    if(sqlite3_prepare_v2(mydb,sql.c_str(),-1,&stmt,nullptr)==SQLITE_OK){
        //This function takes SQL text and asks SQLite: check this SQL, prepare it, and give a statement can use.
        //-1 = read until end of string
        //&stmt = output, SQLite will fill this with the prepared statement.
        
        //1 = replace first ? with name  
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::cout << "User found!\n";
			send(client, name, strlen(name), 0);
        }
        else {
            std::cout << "User not found!\n";
        }

        sqlite3_finalize(stmt);

    }
}




