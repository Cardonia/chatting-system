#include "functions.h"
#include "database.h"
#include "picosha2.h"
#include <iostream>
//#include <thread>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <mutex>
#include <unordered_map>

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>   // for close()
#include <cstring>    // for memset


#include "json.hpp"
using json = nlohmann::json;

Database& db = Database::getInstance("users.db");

std::unordered_map<std::string, int>EventMap = {

    {"REGISTER" , 4},
    {"TOKEN_CHECK" , 2},
    {"LOGIN" , 3},
    {"SEARCH_FRIEND" , 5},
    {"SENT_FRIEND_REQUEST" , 6},
    {"ACCEPT_FRIEND_REQUEST" , 7}

};

std::map<std::string, int> onlineClients; 
// key = token, value = socket
//for storing online clients with token and socket_fd

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











void acceptFriendRequest(int client, std::string msg) {
    std::cout << msg << std::endl;
    json j = json::parse(msg);
    std::cout << "works" << std::endl;
    std::string token = j["token"];
    std::cout << "works1" << std::endl;
    std::string toFriend = j["toName"];
    std::cout << "works2" << std::endl;

    std::string tokenHash = picosha2::hash256_hex_string(token);
    std::cout << toFriend << " wants to accept " << token << "friend request " << std::endl;
    
    db.acceptFriendRequest(toFriend, tokenHash);
    updateAllClientData(client, token);
}


void addFriendRequest(int client, std::string msg) {
	
    std::cout  << msg << std::endl;
    json j = json::parse(msg);
    std::cout << "works" << std::endl;
    std::string token = j["token"];
    std::cout << "works1" << std::endl;
    std::string toFriend = j["toName"];
    std::cout << "works2" << std::endl;

    std::string tokenHash = picosha2::hash256_hex_string(token);
    std::cout << token << " wants to add " << toFriend << " as a friend" << std::endl;
    db.addFriendRequestTable(toFriend, tokenHash);
     
}










void checkToken(int client , std::string msg) {

   
    json j = json::parse(msg);
    std::string tokenFromUser = j["token"];

    std::string tokenHash = picosha2::hash256_hex_string(tokenFromUser);

    if (db.validateToken(tokenHash)) {
        std::cout << "Token matched!" << std::endl;
		json j2;
		j2["event"] = "TOKEN_VALID";
		sendClientMsg(client, j2);
        int socket_fd = static_cast<int>(client);
        onlineClients[tokenHash] = socket_fd;
        updateAllClientData(client, tokenFromUser);
        for (const auto& pair : onlineClients) {
            const std::string& token = pair.first;
            int socket = pair.second;

            std::cout << "Token: " << token << ", Socket: " << socket << std::endl;
        }
    }
    else {
        std::cout << "No match for token" << std::endl;
        json j3;
        j3["event"] = "TOKEN_INVALID";
        sendClientMsg(client, j3);
    }



}

void updateAllClientData(int client , std::string token) {
    friendPendingRequestList(client, token);
}


void LoginHandle(int client , std::string msg) {
    
    std::cout << "Received: " << msg << std::endl;

    json j = json::parse(msg);
    std::string username = j["username"];
    std::string password = j["password"];
    std::string passwordPash = picosha2::hash256_hex_string(password);


        if (db.validateLogin(username, passwordPash)) {
            
            std::cout << "Login OK\n";
            std::string newToken = generateToken();
            std::string newTokeHash = picosha2::hash256_hex_string(newToken);
            db.updateUserToken(username, newTokeHash);

            json j2;
            j2["event"] = "LOGIN_SUCCESS";
			j2["token"] = newToken;
           
			sendClientMsg(client, j2);

            updateAllClientData(client, newToken);

            int socket_fd = static_cast<int>(client);
            onlineClients[newTokeHash] = socket_fd;

        }
        else {
            json j3;
            j3["event"] = "LOGIN_FAILED";
            sendClientMsg(client, j3);
            std::cout << "Wrong username or password\n";
        }
}

void RegisterHandle(int client,std::string msg) {
    std::cout << " register function called" << std::endl;
   

    json j = json::parse(msg);
    std::string username = j["username"];
    std::string password = j["password"];






    

    if (db.userExists(username)) {
        std::cout << "Username exists" << std::endl;

        json j; 
        j["event"] = "REGISTER_USER_EXIST";
   
        
        sendClientMsg(client, j);
        return;
    }
   // else send(client, "2", 1, 0);
    
    std::string token = generateToken();
    json j2;
	j2["event"] = "REGISTER_SUCCESS";
    j2["token"] = token;
    

	sendClientMsg(client, j2);



    std::string passwordHash = picosha2::hash256_hex_string(password);
    std::string tokenHash = picosha2::hash256_hex_string(token);
    std::cout << "SHA-256 of \"" << password << "\": " << passwordHash << std::endl;
    std::cout << "SHA-256 of \"" << token << "\": " << tokenHash << std::endl;

    
    updateAllClientData(client, token);
    
    db.registerUser(username, passwordHash, tokenHash);
	std::cout << "User registered successfully"<<std::endl;
    
    int socket_fd = static_cast<int>(client);
    onlineClients[tokenHash] = socket_fd;


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





void searchForClient(int client , std::string msg) {
	json j = json::parse(msg);
    std::string searchName = j["name"];
    std::cout << "Searching for: " << searchName << std::endl;


   

    std::vector<std::string> names = db.searchUsers(searchName);
       

        json json;
        json["event"] = "SEARCH_FRIEND_RESULT";
        json["names"] = names;  // put vector in JSON

        // Convert to string to send
        
		

		sendClientMsg(client, json);
        

    
}






void readClientMsg(int client, fd_set& master_list) {

    std::cout << "func called " << std::endl;


    uint32_t size = 0; //4 bytes variable
    /* visual
    memory address   0x1000  0x1001  0x1002  0x1003
    value              00       00     00      00
    */
    int received = 0;

    while (received < sizeof(size)) {
        //sizeof(size) is 4 bytes because of uint32_t type is 4 bytes
        //mean 0 < 4
        std::cout << "reading size " << received << std::endl;

        char* start = (char*)&size;
        //pointer for the 4 memory address of size variable. default address is 0x1000 the memory  address
        char* whereToWrite = start + received;
        //address of the memory to write the new data. start is 0x1000 + received(0,1,2,3) = 0x1000,0x1001,0x1002,0x1003
        int howManyLeft = sizeof(size) - received;
        // how many bytes still needed sizeof(size) is 4 - received(0,1,2,3) = 4,3,2,1

        int r = recv(client, whereToWrite, howManyLeft, 0);
        //recive data from client socket store address whereToWrite(0x1000,0x1001,0x1002,0x1003). start from first address 0x1000 till end address 0x1003. howManyLeft(4,3,2,1) bytes to read
        if (r <= 0) {
            close(client);
            FD_CLR(client, &master_list);   // <--- remove it from the list
            std::cout << "client disconnected" << std::endl;
            return;
        }

        received += r;
    }
    size = ntohl(size);
    //convert from network byte order to host byte order (big-endian to little-endian)
    std::cout << "read size " << received << std::endl;
    std::cout << "read finished" << std::endl;

    //now size variable has a number from 0 to 4,228,250,625

    std::vector<char> buffer(size);
    //make a char vector named buffer which the size is depend on (size variable).
    // if size is 50 then this vector has 50 char rows
    received = 0;
    while (received < (int)size) {
        //(int)size convert size which is uint_t to int
        std::cout << "reading msg " << received << std::endl;
        char* start = buffer.data();
        //pointer to vector first row address to start there to write
        char* whereToWrite = start + received; // where to write
        int howManyLeft = size - received;    // how many bytes left to write

        int r = recv(client, whereToWrite, howManyLeft, 0);

        if (r <= 0) {
            close(client);
            FD_CLR(client, &master_list);   // <--- remove it from the list
            std::cout << "client disconnected" << std::endl;
            return;
        }

        received += r;
        // r how many bytes got from client. then add to received to know how many bytes are read by now
    }
    std::cout << "second loop finished " << std::endl;
    std::string msg(buffer.begin(), buffer.end());
    //from index 0 to last index of the vector put in msg as string
    std::cout << "Client says: " << msg << std::endl;
    handleClientEvent(client, msg);


}




void friendPendingRequestList(int client ,const std::string token) {

    std::string tokenHash = picosha2::hash256_hex_string(token);

    int clientId = db.whatUserIdIAM(tokenHash);
    if (clientId == -1) return;
    std::vector<std::string> names = db.friendPendingRequest(clientId);

    json json;
    json["event"] = "FRIEND_REQUEST_PENDING_LIST";
    json["names"] = names;  // put vector in JSON

    // Convert to string to send



    sendClientMsg(client, json);

}