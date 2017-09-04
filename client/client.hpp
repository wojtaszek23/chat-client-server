#ifndef CLIENT
#define CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <thread>

void error(const char *msg);

class Client
{
	public:
	
		int sockfd, portno;
		struct sockaddr_in serv_addr;
		struct hostent* server;
		std::thread* thread_read;
		std::thread* thread_write;
		
		Client();
		void read();
		void write();
		void close();
};

#endif