#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>

void error(const char *msg);

class Server
{
	private:
	
		int id_max;
		int sockfd, portno;
		struct sockaddr_in serv_addr;
		std::string name;
		
		struct Client_Info
		{
			public:
				std::string name;
				int id;
				int sockfd;
				socklen_t addr_len;
				struct sockaddr_in addr;
				std::thread* thread_read;
				std::thread* thread_write;
		};
		std::unordered_map < std::string , Client_Info* > client;
		
		int handshake ( Client_Info *client_new );
		int interpret( Client_Info* client_current, std::string buffer );
		void read( Client_Info *client_current );
		void write( Client_Info *client_current, std::string buffer, bool printing = true );
		void close( Client_Info *client_current);
		
	public:
		
		Server();
		void listen();
};

#endif