#define FD_SETSIZE 1024
#include <iostream>
//#include <thread>

#include "functions.h"
#include "database.h" 
//#include "picosha2.h" 

//linux headers for sochet,bind,lsiten,accept ......
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>   // for close()
#include <cstring>    // for memset


int main() {
    //Database::showTableOnConsole() {
   // Database& db = Database::getInstance("users.db");

    //db.showTableOnConsole();
    
   
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
    local ip / any ip
    local port / 5000

    Send - Receive Buffers / empty
    Remote IP - Port / none (no client connected)
    */

    listen(server_fd, 10);// start listening on port 5000 (server_fd), max 10 clients in queue

    //status / listening

    std::cout << "server listening on port 5000" << std::endl;

	fd_set master_list; //declare master list of sockets
	FD_ZERO(&master_list); //clear the master list from garbage values
	FD_SET(server_fd, &master_list); //add the server socket to the master list
	std::cout << "fd_set is set" << std::endl;

     int max_fd = server_fd;

    while (true) {
		fd_set copy_list = master_list;
		//copy the master list to the temporary list because select() will modify the list we give it

       


      if (select(max_fd + 1, &copy_list, nullptr, nullptr, nullptr) == -1) {
        std::cerr << "select() failed\n";
        break;
    }
        std::cout << "select found activity" << std::endl;

        //if (client == INVALID_SOCKET) { std::cerr << "Accept failed\n";  FD_CLR(client, &master_list);  continue; }
        
        for (int fd = 0; fd <= max_fd; fd++) {
            //loop how many active sockets are there

            if (FD_ISSET(fd, &copy_list)) {

            if (fd == server_fd) {
                //if server_fd had activity which from new client connecting then the sock copied from first index which is the server socket
                //if server socket was equal to itself then true
                // New connection
                std::cout << "detect new client" << std::endl;
                sockaddr_in clientAddr;
                socklen_t clientLen = sizeof(clientAddr);
                int client = accept(server_fd, (sockaddr*)&clientAddr, &clientLen);

                if (client == -1) { std::cerr << "Accept failed\n"; continue; }


                FD_SET(client, &master_list);

                if (client > max_fd) max_fd = client;
                // Update max_fd so select() checks all sockets, including the new client
                std::cout << "connected and added to master list" << std::endl;
                std::cout << "New client connected\n";
            }
            else {
                std::cout << "handling client request" << std::endl;
				//handleClient(sock, "client");

				readClientMsg(fd , master_list);




                }
            }
        }
    }

    close(server_fd);
    

    return 0;
}
