#include "server.hpp"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

Server :: Server()
{
	this->name = "server";
	id_max = 0;
	std::cout<<"Define port: ";
	std::cin>>portno;
	getchar();
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if ( bind ( sockfd, (struct sockaddr *) &serv_addr,
	sizeof(serv_addr) ) < 0 ) 
		error("ERROR on binding");
}

void Server :: listen()
{
	while(1)
	{
		::listen(sockfd,5);

		Client_Info *client_new = new Client_Info;
				
		client_new->addr_len = sizeof ( client_new->addr );
				
		client_new->sockfd = accept ( sockfd, 
			(struct sockaddr *) &client_new->addr, &client_new->addr_len);
					
		if ( client_new->sockfd < 0 ) 
			error("ERROR on accept");
					
		if( handshake ( client_new ) ){
			client_new->thread_read = new std::thread( &Server::read, this, client_new );
			client_new->thread_read->detach();
		}
	}
}

int Server :: handshake ( Client_Info *client_new )
{
	char buf[512];
	memset ( buf , 0 , 512 );
	this->write(client_new, "[server]: Please, type your nick: ");
	if ( ::read ( client_new->sockfd , buf , 511 ) < 0 )
		error("ERROR reading from socket while hand-shaking");
	if ( buf[0] == 0 )
	{
		::close(client_new->sockfd);
		delete client_new;
		std::cout<<"[server]: unrecognized connection aborted"<<std::endl;
		return 0;
	}
	std::string buffer ( buf );
			
	if( client.count(buffer) == 0 ){
		client_new->name = buffer;
		client.insert ( std::make_pair < std::string, Client_Info* > ( buffer.c_str(), &(*client_new) ) );
		buffer = "[server]: Welcome " + client_new->name + "!";
		this->write ( client_new , "" , false );
		
		for ( auto it = client.begin(); it != client.end(); it++ )
		{
			this->write ( it->second , buffer , false );
		}
		std::cout<<buffer<<std::endl;
		buffer = "\nThis is a Lan-TCP/IP Chat made by Wojciech Stanis³awski.\n\
		\r-If you want to exit, type '<exit>'.\n\
		\r-If you want to get list of current clients, type '<show_clients>'.\n\
		\r-If you want to write private message to some client, type:\n\
		\r<that_client_name>: Your message\n\
		\r-If you want to write to all clients, just write what you want,\n\
		\rbut don't do it the way described above then (no '<' as first letter of message)\n";
		this->write ( client_new , buffer , false );
		return 1;
	}
	else
	{
		buffer = "[server]: " + buffer + "- name reserved, connection closed";
		this->write ( client_new , buffer );
		::close(client_new->sockfd);
		delete client_new;
		return 0;
	}
			
}

int Server :: interpret( Client_Info* client_current, std::string buffer )
{
	if(buffer[0]=='<'){
		if ( 1 == buffer.find("exit>") )
		{
			buffer = "[server]: Goodbye " + client_current->name + "!";
			for ( auto it = client.begin(); it != client.end(); it++ )
			{
				write(it->second , buffer , false);								
			}
			std::cout<<buffer<<std::endl;
			return 1;
		}
		else if ( 1 == buffer.find("show_clients>") )
		{
			buffer = "[server]: clients:";
			this->write ( client_current , buffer );
			for ( auto it = client.begin(); it != client.end(); it++ )
			{
				this->write ( client_current , it->second->name );
			}
			return 0;
		}
		else
		{
			for ( auto it = client.begin(); it != client.end(); it++ )
			{
				if( 1 == buffer.find(it->second->name) ){
					if( ( it->second->name.size() + 1 ) == buffer.find(">:") ){
						buffer = buffer.substr( it->second->name.size() + 3 );
						std::cout<<client_current->name<<"->"<<it->second->name<<":"<<buffer<<std::endl;
						buffer = client_current->name + ":" + buffer;
						this->write(it->second, buffer, false);
						return 0;
					}
				}
			}
			buffer = "[server]: " + buffer.substr ( 1 , buffer.find ( ">" ) - 1 ) + " - there's no such client or command";
			this->write ( client_current , buffer );
			return 0;
		};
	}
	buffer = client_current->name + ": " + buffer;
	for ( auto it = client.begin(); it != client.end(); it++ )
	{
		if( it->second->name != client_current->name )
			write(it->second , buffer , false);								
	}
	std::cout<<buffer<<std::endl;
	return 0;
}

void Server :: read( Client_Info *client_current )
{
	int n;
	char buf[512];
	while(1)
	{
		memset ( buf , 0 , 512 );
		if ( ::read( client_current->sockfd , buf , 511 ) < 0 )
			error("ERROR reading from socket");
		if ( buf[0] == 0 )
		{
			std::cout<<client_current->name<<" disconnected "<<std::endl;
			return close(client_current);
		}
		std::string buffer ( buf );
		if ( interpret( client_current, buffer ) )
			return close(client_current);
	}				
}

void Server :: write( Client_Info *client_current, std::string buffer, bool printing /* = true */ )
{
	buffer+="\n";
	int n;
	char buf[512];
	memset ( buf , 0 , 512 );
	memcpy ( buf , buffer.c_str() , strlen ( buffer.c_str() ) ); 
	n = ::write ( client_current->sockfd , buf , strlen(buf) );
	if (n < 0) 
		error("ERROR writing to socket");
	if ( printing )
		std::cout<<buffer;	
}

void Server :: close( Client_Info *client_current)
{
	::close(client_current->sockfd);	
	client.erase(client_current->name);
	delete client_current;
}