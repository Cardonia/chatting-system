#include <iostream>
#include <unordered_map>

//linux headers for sochet,bind,lsiten,accept ......
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>  
#include "functions.h"

//create struct for each client to store its data
struct clientState{
    uint32_t size = 0;
    int received = 0;
    std::vector<char> buffer;
    bool readingSize = true;
    time_t lastActivity;
};

std::unordered_map <int , clientState> clients;
//map to store client socket with its clientState

void readClientMsg(int fd, fd_set& socket_list);//fucntion to read clients messages 

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { std::cerr << "Server Socket creation failed\n" << std::endl;   return 1; }
    std::cerr << "Server Socket successfully created  " << server_fd << std::endl;
    //create server socket

    sockaddr_in addressHolder{};
    addressHolder.sin_family = AF_INET;
    addressHolder.sin_addr.s_addr = INADDR_ANY;
    addressHolder.sin_port = htons(5000);
    //make a structure to hold  ip , ip type , port

    if (bind(server_fd, (sockaddr*)&addressHolder, sizeof(addressHolder)) == -1) {
        std::cerr << "Bind failed\n";
        close(server_fd);
        return 1;
    }
    //bind the server socket with the pre made struct , quit if bind failed 

    listen(server_fd, 10);
    // start listening on port 5000 , max 10 clients in queue

    std::cerr << "server listening on port 5000" << std::endl;
    log("Server Started");

	fd_set socket_list; 
    FD_ZERO(&socket_list);
	FD_SET(server_fd, &socket_list); 
    //create socket list then put server socket into it

    int max_fd = server_fd;
    //track last socket (example 3 , server socket)

    while (true) {
		fd_set copy_list = socket_list;
		//copy the socket list to the temporary list because select() will modify the list we give it

        if (select(max_fd + 1, &copy_list, nullptr, nullptr, nullptr) == -1) {
            std::cerr << "select() failed\n";
            break;
        }
        //sleep and watch for sockets activity   

        std::cout << "select found activity" << std::endl;

        time_t currentTime = time(nullptr);
        //store current time
        for (auto thisSocket = clients.begin(); thisSocket != clients.end(); ) {
            //loop through each one in clients  as thisSocket key-value object

            auto& state = thisSocket->second;
            bool midMessage = !state.readingSize || state.received > 0;

            if (midMessage && currentTime - state.lastActivity > 5) {
                //run if client was idle for more than 5s
                disconnectClient(thisSocket->first);
                FD_CLR(thisSocket->first, &socket_list);
                thisSocket = clients.erase(thisSocket);
                std::cout << "client timed out\n";
            } 
            else ++thisSocket;  // move to next client
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            //loop how many active sockets are there

            if (FD_ISSET(fd, &copy_list)) {
            //check if fd sockets is set and has data (0-3) and same fd is in copy_list then run

                if (fd == server_fd) {
                    //if the fd was equal to server_fd which is thats the sever socket that has activity
                    //if server_fd had activity which is from new client connection
                    // new connection
                    std::cout << "detect new client" << std::endl;
                    sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    int client = accept(server_fd, (sockaddr*)&clientAddr, &clientLen);
                    
                    if (client == -1) { std::cerr << "Accept failed\n"; continue; }

                    //put the new client to socket_list
                    FD_SET(client, &socket_list);

                    if (client > max_fd) max_fd = client;
                    // update max_fd so select() checks all sockets, including the new client
                    std::cout << "connected and added to socket list" << std::endl;
                    std::cout << "New client connected\n";
                    log("New Client Connected   FD = "+std::to_string(client));

                    clients[client].lastActivity = time(nullptr);
                }
                else {
                    //if it wasn't server fd then its old client socket activity
                    std::cout << "handling client request" << std::endl;
                    readClientMsg(fd , socket_list);
                }
            }
        }
    }
    close(server_fd);
    return 0;
}

void readClientMsg(int fd, fd_set& socket_list) {
    auto& state = clients[fd];
    //get the client state 
    if(state.readingSize){
        //run if the readingSize of the client is true
        //state object of it

        int recev = recv(fd,((char*)&state.size) + state.received, 4 - state.received, 0);
        //recv read data from socket
        //((char*)&state.size) is pointer to 4 byte memory address
        //state.received to tell where to start , and read for how many bytes 
        
        state.lastActivity = time(nullptr);
        //update socket timeout to current time

        //close and delete the socket if there is no data
        if(recev <= 0){
            disconnectClient(fd); 
            FD_CLR(fd,&socket_list);
            clients.erase(fd); 
            return;
        }

        state.received += recev;
        //track the recevied msg size data


        //if msg size data recevied well 4bytes
        if(state.received == 4){
            state.size = ntohl(state.size);
            //ntohl converts byte from big endian(network) to system endian
            state.buffer.resize(state.size);
            //set size the buffer vector with state.size
            state.received = 0;
            //set to default
            state.readingSize = false;
            //set readingSize to false to stop reciving size data to read actuall msg
        }
        return;
        //stay here as long as size data < 4 bytes
    }
    //clsoe client socket is sent big msg
    if (state.size > 10000) { //10,000 byte max
        disconnectClient(fd);
        FD_CLR(fd, &socket_list);
        clients.erase(fd);
        std::cout << "client sent too big message\n";
        return;
    }
    //read message
    int recev = recv(fd,state.buffer.data()+state.received,state.size - state.received,0);
    //state.buffer.data() pointer to first byte address of the buffer vector 
    //since its buffer we use data() to get first value address

    state.lastActivity = time(nullptr);
    //update socket timeout to current time

    if (recev <= 0) { disconnectClient(fd); FD_CLR(fd, &socket_list); clients.erase(fd); return; }
    
    state.received += recev;

     if (state.received == state.size) {
        std::string msg(state.buffer.begin(), state.buffer.end());
        //convert buffer vector<char> to string

        //handle the msg
        handleClientEvent(fd, msg);

        // reset for next message
        state.readingSize = true;
        state.received = 0;
        state.buffer.clear();
        state.size = 0;
    }
}