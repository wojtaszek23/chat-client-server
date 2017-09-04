#include "client.hpp"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

Client :: Client()
{
	std::string host_name;
	std::cout<<"Define hostname: ";
	std::cin>>host_name;
	server = gethostbyname( host_name.c_str() );
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	std::cout<<"Define port: ";
	std::cin>>portno;
	getchar();
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)server->h_addr, 
		(char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	else
	{	
		thread_read = new std::thread(&Client::read, this);
		thread_write = new std::thread(&Client::write, this);
		thread_write->detach();
		thread_read->join();
		delete thread_write;
	};
}

void Client :: read()
{
	int n;
	char buf[512];
	while(1)
	{
		memset ( buf , 0 , 512 );
		n = ::read( sockfd , buf , 511 );
		if (n < 0) 
			error("ERROR reading from socket");
		std::string buffer ( buf );
		std::cout<<buffer;
					
		if(buf[0]==0)
		{
			std::cout<<"connection with server aborted";
			::close(sockfd);
			break;
		};
	}
}
		
void Client :: write()
{
	int n;
	std::string buffer;
	char buf[512];
	while(1)
	{
		getline (std::cin , buffer );
		memset ( buf , 0 , 512 );
		memcpy ( buf , buffer.c_str() , strlen ( buffer.c_str() ) ); 
		n = ::write ( sockfd , buf , strlen(buf) );
		if (n < 0) 
			error("ERROR writing to socket");
	}
}
		
void Client :: close()
{
	::close(sockfd);
}