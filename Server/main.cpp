#include <iostream>
#include <unordered_map>

#include "functions.h"
#include "database.h" 

//linux headers for sochet,bind,lsiten,accept ......
#include <sys/select.h>
/*
add 
functions select() 
macros fd_set, FD_SET, FD_ZERO, FD_ISSET.
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>   // for close()
#include <cstring>    // for memset
///////////////////////////////////////////////////////////////////////////
struct clientState{

    uint32_t size = 0;
    int received = 0;
    std::vector<char> buffer;
    bool readingSize = true;
    time_t lastActivity;
    
};

std::unordered_map <int , clientState> clients;

void readClientMsg(int fd, fd_set& socket_list);


///////////////////////////////////////////////////////////////////////////

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { std::cerr << "Socket creation failed\n" << std::endl;   return 1; }
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

    if (bind(server_fd, (sockaddr*)&addressHolder, sizeof(addressHolder)) == -1) {
        std::cerr << "Bind failed\n";
        close(server_fd);
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
    local ip / any ip    local ip / any ip

    local port / 5000

    Send - Receive Buffers / empty
    Remote IP - Port / none (no client connected)
    */

    listen(server_fd, 10);
    // start listening on port 5000 (server_fd), max 10 clients in queue

    std::cerr << "server listening on port 5000" << std::endl;
    log("Server Started");
	fd_set socket_list; 
    //declare socket list of sockets
	//make a list for storing sockets 

    FD_ZERO(&socket_list); //clear the socket list from garbage values
	FD_SET(server_fd, &socket_list); 
    //add the server socket to the socket list
    //example server sockets is 3 so list contains (3)
	
    int max_fd = server_fd;
    //track last socket (3)

    while (true) {
		fd_set copy_list = socket_list;
		//copy the socket list to the temporary list because select() will modify the list we give it

        if (select(max_fd + 1, &copy_list, nullptr, nullptr, nullptr) == -1) {
            std::cerr << "select() failed\n";
            break;
        }
        // max_fd + 1 = 4.   checks for all sockets from 0 to 3 
        //sleep here untill it find an activity from one of them
        //it skip 0 - 2 because they are not socket

        std::cout << "select found activity" << std::endl;



        time_t currentTime = time(nullptr);
        //store currnt time
        for (auto thisSocket = clients.begin(); thisSocket != clients.end(); ) {
            //loop through each one in clients  as thisSocket

            auto& state = thisSocket->second;

            bool midMessage = !state.readingSize || state.received > 0;

            //int fd = thisSocket->first;
            //get first part value of the unourdered map (int fd, fd_set& socket_list)
            //get client socket fd 

            //clientState& state = thisSocket->second;
            //get second part value of the unourdered map
            //get the stuct as refernce not copy
            //so we can edit the data directly

            //if (currentTime - state.lastActivity > 5) { // 5 second timeout
            if (midMessage && currentTime - state.lastActivity > 5) {
                //close(fd);
                close(thisSocket->first);
                FD_CLR(thisSocket->first, &socket_list);
                //FD_CLR(fd, &socket_list);
                thisSocket = clients.erase(thisSocket);
                // erase and move to next
                std::cout << "client timed out\n";
            } 

            else {++thisSocket; } // move to next client
        }







        for (int fd = 0; fd <= max_fd; fd++) {
            //loop how many active sockets are there

            if (FD_ISSET(fd, &copy_list)) {
            //check if fd sockets is set and has data (0-3) and same fd is in copy_list then run
            //FD_ISSET(fd, &copy_list) == true

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
                    log("New Client Connected   FD = "+client);

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
    //make variable state and figure the type auto
    //make the struct for the fd and put it in the unordered list

    if(state.readingSize){
        //run if the readingSize of the client is true
        //state object of it

        int recev = recv(fd,((char*)&state.size) + state.received, 4 - state.received, 0);
        //recv data from fd socket
        //&state.size address of the start.size
        //char* pointer to byte address

        //uint32_t state.size = 0
        //memory: 0x1000  0x1001  0x1002  0x1003
        //values:  00     00     00     00

        //char* store 0x1000 value

        //0x1000    0x1000 + state.received (1)
        //00        00

        //read how many bytes     4 - state.received

        
        state.lastActivity = time(nullptr);
        //update socket timeout to current time

        //close and delete the socket if there is no data
        if(recev <= 0){
            close(fd); 
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
        close(fd);
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

    if (recev <= 0) { close(fd); FD_CLR(fd, &socket_list); clients.erase(fd); return; }
    
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

