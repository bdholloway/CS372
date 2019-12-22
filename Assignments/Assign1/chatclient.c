/************************ 
 * Branden Holloway
 * CS372, Project 1
 * July 27, 2019
 ************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


//[1] https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html
//[2] https://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/types.h.html
//[3] http://pubs.opengroup.org/onlinepubs/009695399/basedefs/netdb.h.html

//UserName and ServerName Variables
char UserName[8];
char ServerName[10];

void GetUserName(){
	printf("Enter a Valid username of 8 chars or less\n > ");
	scanf("%s", UserName);
	if(strlen(UserName) > 8){
		printf("UserName is to long, Try Again\n");
		GetUserName();
	}
}

/**********************************************
** CreateAdd()
** Gets informaion <address> <PortNumber>
** The struct addrinfo comes from <netbd.h>, see 1 
**********************************************/
struct addrinfo* CreateAdd(char* address, char* PortNumber){             
	struct addrinfo socket;
    struct addrinfo *Client;
	
	//Comes from <netbd.h>, Socket Type
	socket.ai_socktype = SOCK_STREAM;

	//Came from Beej's guide. Checks that getaddrinfo does not return a 0. If does exit program.
	int check;
	if((check = getaddrinfo(address, PortNumber, &socket, &Client)) != 0){   
		fprintf(stderr, "Incorrect Port #\n", gai_strerror(check));
		exit(1);
	}
	return Client;
}

/**********************************************
** CreateSocket()
** 
** Contiuned use of Beej's guide, feed address info into socket creation function 
**********************************************/
int CreateSocket(struct addrinfo* Client){           
	int sock;                                                 

	//If Sock returns -1, then exit program
	if ((sock = socket(Client->ai_family, Client->ai_socktype, Client->ai_protocol)) == -1){
		fprintf(stderr, "Socket could not be created\n");
		exit(1);
	}
	return sock;
}

/**********************************************
** ConnectSocket()
** Connects Socket and checks for error
** Beej's Guide used
**********************************************/
void ConnectSocket(int sock, struct addrinfo * Client){
	int check;
	if ((check = connect(sock, Client->ai_addr, Client->ai_addrlen)) == -1){
		fprintf(stderr, "Connection Error\n");
		exit(1);
	}
}

/**********************************************
** Chat()
** Runs the Receiving and Sending of messages as well as error control
** Beej's Guide Used
**********************************************/
void Chat(int sock, char * username, char * servername){ 
    int Errors = 0;   
	int status;

	//Input/Output message holders
	char InputMsg[500];
	char OutputMsg[500];
	//Clears Buffers
    memset(InputMsg, 0 ,sizeof(InputMsg));
	memset(OutputMsg, 0 ,sizeof(OutputMsg));
	fgets(InputMsg, 500, stdin);
    
	while(1){		
		//Will display username and cursor for messages

		printf("\033[0;31m%s>\033[0m ", username);
		fgets(InputMsg, 500, stdin);

		//Gives user a way to exit chat by typing \quit
		if (strcmp(InputMsg, "\\quit\n") == 0)
			break;
		
		Errors = send(sock, InputMsg, strlen(InputMsg), 0);
		
		if(Errors == -1){
				fprintf(stderr, "Sending message encountered an error\n");
				exit(1);
		}
		//From beej's guide
		status = recv(sock, OutputMsg, 500, 0);
		
		if (status == -1){
			fprintf(stderr, "Receiving message encountered an error\n");
			exit(1);
		}
        
		else if (status == 0){                              
			printf("Chat has ended\n");
			break;
		}
		//Print message
		else                                                      
			printf("\033[0;32m%s>\033[0m%s\n", servername, OutputMsg);
	}
	//Closes the connection after chat is finished
	close(sock);                                  
	printf("Chat has ended\n");
}

/**********************************************
** Information()
** Keeps track of the information being used
** 
**********************************************/
void Information(int sock, char* UserName, char* ServerName){	        
	int sendingcode = send(sock, UserName, strlen(UserName), 0);	
	int receivingcode = recv(sock, ServerName, 10, 0);
}

/**********************************************
** main()
** Calls the appropriate functions to create client and run chat
** 
**********************************************/
int main(int argc, char *argv[]){                              
    //Checks that number of arguments given is correct
	if(argc != 3){                                                      
		fprintf(stderr, "Invalid Address/Port number\n");
		exit(1);
	}
	GetUserName();
	struct addrinfo* Client = CreateAdd(argv[1], argv[2]);   
	int sock = CreateSocket(Client);
	ConnectSocket(sock, Client);     
	Information(sock, UserName, ServerName); 
	Chat(sock, UserName, ServerName); 
	//Free struct
	freeaddrinfo(Client);
}
