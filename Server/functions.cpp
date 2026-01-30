#include "functions.h"
#include "database.h"
#include "picosha2.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <unordered_map>

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>   
#include <cstring>    


#include "json.hpp"
using json = nlohmann::json;

Database& db = Database::getInstance("users.db");

std::unordered_map<std::string, int>EventMap = {

    {"REGISTER" , 4},
    {"TOKEN_CHECK" , 2},
    {"LOGIN" , 3},
    {"SEARCH_FRIEND" , 5},
    {"SEND_FRIEND_REQUEST" , 6},
    {"ACCEPT_FRIEND_REQUEST" , 7}

};

std::vector<std::string> logBuffer;
//storing logs

std::map<std::string, int> onlineClients; 
// key = token, value = socket
//for storing online clients with token and socket_fd
std::map<int , std::string> onlineClientsRE;

void sendClientMsg(int client, json jsonObject) {

    std::string jsonString = jsonObject.dump();
    // 4-byte length prefix
    //uint32_t length = htonl(jsonString.size());
    uint32_t length = htonl(static_cast<uint32_t>(jsonString.size()));

    // Combine length + JSON into one buffer
    std::vector<char> packet(4 + jsonString.size());
    std::memcpy(packet.data(), &length, 4);
    std::memcpy(packet.data() + 4, jsonString.data(), jsonString.size());


    send(client, packet.data(), packet.size(), 0);
    std::cout << "Sent to client: " << jsonString << std::endl;
}




void handleClientEvent(int client, std::string msg) {
	std::cout << "------------------------------------------------"<< std::endl;
        
        json j=json::parse(msg);
        std::string event = j["event"];
        

		int eventCode = EventMap[event];

        switch (eventCode) {
        case 0: std::cout << "case 0 called\n";

            break;
        case 2:
            std::cout << "case TOKEN_CHECK called\n";
            checkToken(client, msg);
            break;


        case 3:
            std::cout << "case LOGIN called\n";
            LoginHandle(client,msg);
            break;

        case 4:
            std::cout << "case register called\n";
            RegisterHandle(client,msg);
            break;

        case 5:
            std::cout << "case SEARCH_FRIEND called\n";
            searchForClient(client,msg);
            break;

        case 6:
            std::cout << "case SENT_FRIEND_REQUEST called\n";
            addFriendRequest(client , msg);
            break;

        case 7:
            std::cout << "case ACCEPT_FRIEND_REQUEST called\n";
            acceptFriendRequest(client,msg);
            break;

        default: std::cout << "unknown code\n";
        }
    

}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



void checkToken(int client , std::string msg) {

    log("Check Token , FD = "+client);
    json j = json::parse(msg);
    std::string tokenFromUser = j["token"];
    std::string tokenHash = picosha2::hash256_hex_string(tokenFromUser);

    if (db.validateToken(tokenHash)) {
        std::cout << "Token matched!" << std::endl;
		json j2;
		j2["event"] = "TOKEN_VALID";
		sendClientMsg(client, j2);

        onlineClients[tokenHash] = client;
        onlineClientsRE[client] = tokenHash;
        updateAllClientData(client, tokenHash);

        log("Check Token = Valid , FD = "+std::to_string(client));

        //###########################################
        for (const auto& pair : onlineClients) {
            const std::string& token = pair.first;
            int socket = pair.second;
            std::cout <<"online map : "<< "Token: " << token << ", Socket: " << socket << std::endl;
        }   
        //###########################################
    }
    else {
        std::cout << "No match for token" << std::endl;
        json j3;
        j3["event"] = "TOKEN_INVALID";
        sendClientMsg(client, j3);
        log("Check Token = Invalid , FD = "+std::to_string(client));
    }
}



///////////////////////////////////////////////////////


void RegisterHandle(int client,std::string msg) {
    std::cout << " register function called" << std::endl;
    log("Register , FD = "+std::to_string(client));

    json j = json::parse(msg);
    std::string username = j["username"];
    std::string password = j["password"];

    if (db.userExists(username)) {
        std::cout << "Username exists" << std::endl;

        json j; 
        j["event"] = "REGISTER_USER_EXIST";
   
        sendClientMsg(client, j);
        log("Register = Failed (User Already Exist) , FD = "+std::to_string(client));
        return;
    }
   
    std::string token = generateToken();
    json j2;
	j2["event"] = "REGISTER_SUCCESS";
    j2["token"] = token;
    
	sendClientMsg(client, j2);

    std::string passwordHash = picosha2::hash256_hex_string(password);
    std::string tokenHash = picosha2::hash256_hex_string(token);

    db.registerUser(username, passwordHash, tokenHash);
	std::cout << "User registered successfully"<<std::endl;
    
    onlineClients[tokenHash] = client;
    onlineClientsRE[client] = tokenHash;

    log("Register = Success, FD = "+std::to_string(client));
}



///////////////////////////////////////////////////////




void LoginHandle(int client , std::string msg) {
    log("Login , FD = "+std::to_string(client));
    std::cout << "Received: " << msg << std::endl;

    json j = json::parse(msg);
    std::string username = j["username"];
    std::string password = j["password"];
    std::string passwordPash = picosha2::hash256_hex_string(password);
    
        if (db.validateLogin(username, passwordPash)) {
            
            std::cout << "Login OK\n";
            log("Login = Success, FD = "+std::to_string(client));

            std::string newToken = generateToken();
            std::string newTokeHash = picosha2::hash256_hex_string(newToken);
            db.updateUserToken(username, newTokeHash);

            json j2;
            j2["event"] = "LOGIN_SUCCESS";
			j2["token"] = newToken;
           
			sendClientMsg(client, j2);

            updateAllClientData(client, newTokeHash);

            onlineClients[newTokeHash] = client;
            onlineClientsRE[client] = newTokeHash;

        }
        else {
            log("Login = Failed , FD = "+std::to_string(client));
            json j3;
            j3["event"] = "LOGIN_FAILED";
            sendClientMsg(client, j3);
            std::cout << "Wrong username or password\n";
        }
}




///////////////////////////////////////////////////////////





std::string generateToken() {
    srand(time(0));//for token generation
    std::string token;
    token.clear();
    token.reserve(10); // reserve(store) space for 10 characters
    for (int i = 0; i < 10; i++) {
        char c = 49 + rand() % (122 - 49 + 1);
        token = token + c;
    }
    std::cout << "Generated Token: " << token << std::endl;
    return token;
}




////////////////////////////////////////////////////////////








void searchForClient(int client , std::string msg) {
    log("Search For Friend , FD = "+std::to_string(client));
	json j = json::parse(msg);
    std::string searchName = j["name"];
    std::cout << "Searching for: " << searchName << std::endl;

    std::string tokenHash;
	auto it = onlineClientsRE.find(client);
	//return iterator(like pointer thing!!) if the fd exist in map
	//iterator points to the element if found, else it point to last fake element by default
	if (it != onlineClientsRE.end()) {
		//checks if the fd was found, if it wanst last fake element then it found it
	    tokenHash = it->second;
	}

    json result = db.searchUsers(searchName,tokenHash);

        json json;
        json["event"] = "SEARCH_FRIEND_RESULT";
        json["names"] = result["names"];
        json["names_id"] = result["names_id"];
		sendClientMsg(client, json);
}





/////////////////////////////////////////////////////





void addFriendRequest(int client, std::string msg) {
    json j = json::parse(msg);
    std::string token = j["token"];
    int toFriendId =  j["toId"];
    log("Add friend request to " + std::to_string(toFriendId) + " id , FD = " + std::to_string(client));
    std::string tokenHash = picosha2::hash256_hex_string(token);
    std::cout << token << " wants to add " << toFriendId << " ID as a friend" << std::endl;
    db.addFriendRequestTable(toFriendId, tokenHash);

    
}




/////////////////////////////////////////////////////////




void acceptFriendRequest(int client, std::string msg) {
    /*
    json j = json::parse(msg);
    std::string token = j["token"];
    int toFriendId = j["toId"];

    std::string tokenHash = picosha2::hash256_hex_string(token);
    std::cout << toFriend << " wants to accept " << token << "friend request " << std::endl;
    
    db.acceptFriendRequest(toFriendId, tokenHash);
    updateAllClientData(client, token);
*/}















void updateAllClientData(int client , std::string token) {
    friendPendingRequestList(client, token);
}
















void friendPendingRequestList(int client ,const std::string tokenHash) {

    int clientId = db.whatUserIdIAM(tokenHash);

    std::cout<<"Debug: userID for friendPendingRequestList: "<<clientId<<'\n';
    if (clientId == -1) return;
    json result = db.friendPendingRequest(clientId);

    json json;
    json["event"] = "FRIEND_REQUEST_PENDING_LIST";
    json["names"] = result["names"];
    json["names_id"] = result["names_id"];

    // Convert to string to send



    sendClientMsg(client, json);

}




/////////////////////////////////////////////////
////////////////////////////////////////////////



void log(const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    std::string logLine = ss.str();
    logLine+=" / "+msg;
    logBuffer.push_back(logLine);

    if (logBuffer.size() >= 10) { 
        std::ofstream file("server.log", std::ios::app);
        for (auto& m : logBuffer)
            file << m << '\n';
        logBuffer.clear();
    }
}