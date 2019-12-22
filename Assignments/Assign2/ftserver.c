/*********************************
 * Branden Holloway
 * CS372, Project #2
 * August 12th, 2019
 ********************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>

// Beej's guide is used oftern throughout program
// Project 1 peices also used throughout program
// Some other resources used
// struct addrinfo http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
// https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html
// https://beej.us
// https://www.geeksforgeeks.org/socket-programming-cc/
// http://man7.org/linux/man-pages/man2/bind.2.html


/*************************************
 * CreateAddress(char)
 * Create Address information
 ************************************/
struct addrinfo * CreateAddress(char* port){   
	int status;
	struct addrinfo hints, *Socket;
	//Clear Buffer
	memset(&hints, 0, sizeof hints);
	//Variables come from dirent.h struct        
	hints.ai_family = AF_INET;                  
	hints.ai_socktype = SOCK_STREAM;        
	hints.ai_flags = AI_PASSIVE;
	if((status = getaddrinfo(NULL, port, &hints, &Socket)) != 0){       
		//print error to stderr if not 0
		fprintf(stderr, "Error. Enter correct port.\n", gai_strerror(status));
		exit(1);
	}
	return Socket;
}

/*************************************
 * CreateAddIP(char, char)
 * Create the information for the address, comes from beej's guide
 ************************************/
struct addrinfo * CreateAddIP(char* IPAddress, char* port){      
	int status;                                                                                                 
	struct addrinfo hints, *Socket;
	//Clear buffer
	memset(&hints, 0, sizeof hints);
	//Variables come from dirent.h struct
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if((status = getaddrinfo(IPAddress, port, &hints, &Socket)) != 0){
		//Send error to stderr if one comes up
		fprintf(stderr, "Error. Please enter the correct port\n", gai_strerror(status));
		exit(1);
	}
	return Socket;
}

/*************************************
 * CreateSocket(struct)
 * Feed info into createsocket, from beej's guide
 ************************************/
int CreateSocket(struct addrinfo * Socket){               
	int sockfd;
	//If socket comes back to -1 through error                                                     
	if ((sockfd = socket((struct addrinfo *)(Socket)->ai_family, Socket->ai_socktype, Socket->ai_protocol)) == -1){
		//Print error to stderr
		fprintf(stderr, "Error. Socket was not created.\n");
		exit(1);
	}
	return sockfd;
}

/*************************************
 * ConnectSocket(int, struct)
 * Used to make connection
 ************************************/
void ConnectSocket(int sockfd, struct addrinfo * Socket){             
	int status;
	if((status = connect(sockfd, Socket->ai_addr, Socket->ai_addrlen)) == -1){
		//Print error to stderr if one arises
		fprintf(stderr, "Error in connection.\n");
		exit(1);
	}
}

/*************************************
 * BindSocket(int, struct)
 * From beej's guide
 ************************************/
void BindSocket(int sockfd, struct addrinfo * Socket){                
	if (bind(sockfd, Socket->ai_addr, Socket->ai_addrlen) == -1) {
		close(sockfd);
		//print error to stderr
		fprintf(stderr, "Error in binding socket\n");
		exit(1);
	}
}

/*************************************
 * ListenSocket(int)
 * Used to listen
 ************************************/
void ListenSocket(int sockfd){                     
	if(listen(sockfd, 5) == -1){
		close(sockfd);
		//If error comes up, print to stderr
		fprintf(stderr, "Error. Cannot listen.\n");
		exit(1);
	}
}

/*************************************
 * GetFiles(char)
 * Opens directory and gets files
 ************************************/
int GetFiles(char** files){ 
	//https://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html
	DIR* d;                                 
	struct dirent * dir;
	d = opendir(".");
	int i = 0;
	if(d){
		//Loop through while not NULL
		while ((dir = readdir(d)) != NULL){    
			if (dir->d_type == DT_REG){
				strcpy(files[i], dir->d_name);
				i++;
			}
		}
		//Close directory when finished
		closedir(d);
	}
	return i;
}

/*************************************
 * CreateString(int)
 * Holds directory files
 ************************************/
char ** CreateString(int size){                      
	char ** array = malloc(size*sizeof(char *));
	int i;
	for(i = 0; i < size; i++){
		array[i] = malloc(100*sizeof(char));
		//Clear out buffer
		memset(array[i],0,sizeof(array[i]));
	}
	return array;
}

/*************************************
 * ClearString(char, int)
 * Called to clear string array
 ************************************/
void ClearString(char ** array, int size){     
	int i;
	for(i = 0; i < size; i++){
		//Loop through each part in array and clear said part
		free(array[i]);
	}
	//Finally free array
	free(array);
}

/*************************************
 * CheckFile(char, int, char)
 * Checks file, returns either a 1 for true or a 0 for false
 * 
 ************************************/
int CheckFile(char ** files, int numFiles, char * filename){     
	//Set File to false initially
	int File = 0;                
	int i;
	//Loop through to check if the File is there
	for(i = 0; i < numFiles; i++){             
		if(strcmp(files[i], filename) == 0){
			//Change File to true
			File = 1;
		}
	}
	return File;           
}

/*************************************
 * SendFile(char, char, char)
 * Used to send the file
 ************************************/
void SendFile(char* IPAddress, char * port, char * filename){               
	sleep(2);
	struct addrinfo * Socket;
	//Call to create socket
	Socket = CreateAddIP(IPAddress, port);      
	int dataSocket;
	dataSocket = CreateSocket(Socket);                                               
	ConnectSocket(dataSocket, Socket);
	
	char buffer[2000];
	//Clear buffer                                                                         	
	memset(buffer, 0, sizeof(buffer));
	
	int fd;
	//open file
	fd = open(filename, O_RDONLY); 
	//Loop used to read through file                                                     
	while (1) {
		int SizeBytes = read(fd, buffer, sizeof(buffer) - 1);                        
		if(SizeBytes == 0) 
			break;
		if(SizeBytes < 0) {
			//Print error to stderr
			fprintf(stderr, "Error. Could not read file.\n");
			return;
		}

		void* hold = buffer;
		while(SizeBytes > 0) {
			int WrittenBytes = send(dataSocket, hold, sizeof(buffer),0);
			if(WrittenBytes < 0) {
				fprintf(stderr, "Error in writing\n");
				return;
			}
			SizeBytes -= WrittenBytes;
			hold += WrittenBytes;
		}
		//Clears buffer out
		memset(buffer, 0, sizeof(buffer));             
	}
	//Clear out buffer
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "__done__");
	//Send buffer
	send(dataSocket, buffer, sizeof(buffer),0);
	
	//Close dataSocket and then free address info
	close(dataSocket);          
	freeaddrinfo(Socket);
}

/*************************************
 * SendDirectory(char, char, char, int)
 * Fucntion used to send directory
 ************************************/
void SendDirectory(char * IPAddress, char * port, char ** files, int totalNum){      
	sleep(2);
	struct addrinfo * Socket;
	Socket = CreateAddIP(IPAddress, port);
	int dataSocket;
	dataSocket = CreateSocket(Socket);
	ConnectSocket(dataSocket, Socket);
	int i;
	//Loop through to get total num of files
	for(i = 0; i < totalNum; i++){  
		//send data                   
		send(dataSocket, files[i], 100 ,0);                 
	}
	char* done = "done";
	send(dataSocket, done, strlen(done), 0);
	
	//close dataSocket and then free address info
	close(dataSocket);
	freeaddrinfo(Socket);
}

/*************************************
 * AcceptConnection(int)
 * 
 * Structure for this came from Beejs guide
 ************************************/
void AcceptConnection(int new_fd){	            
	char * good = "ok";
	char * bad = "bad";
	char port[100];
    char command[500];
    //clears buffer
	memset(port, 0, sizeof(port));              
	recv(new_fd, port, sizeof(port)-1, 0);
	send(new_fd, good, strlen(good), 0);
	//clears buffer
	memset(command, 0, sizeof(command));            
	recv(new_fd, command, sizeof(command)-1, 0);
	send(new_fd, good, strlen(good),0);
	char IPAddress[100];
	//clears buffer
	memset(IPAddress, 0, sizeof(IPAddress));
	recv(new_fd, IPAddress, sizeof(IPAddress)-1,0);
	printf("New connection from %s\n", IPAddress);
    //Only want single file
    if(strcmp(command, "g") == 0){              
		send(new_fd, good, strlen(good), 0);
		char filename[100];
		//clears buffer
		memset(filename, 0, sizeof(filename));
		recv(new_fd, filename, sizeof(filename) - 1, 0);
		printf("File: %s requested \n", filename);
		
		char** files;
		files = CreateString(500);
		int numFiles, findFile;
		//Get the file
		numFiles = GetFiles(files);  
		//Check the file       
		findFile = CheckFile(files, numFiles, filename);
		if(findFile){
			printf("File found, sending %s to client\n", filename);
			char * file_found = "File found";
			send(new_fd, file_found, strlen(file_found), 0);
			
			char new_filename[100];
			//clears buffer
			memset(new_filename, 0, sizeof(new_filename));
			strcpy(new_filename, "./");
			char * end = new_filename + strlen(new_filename);
			end += sprintf(end, "%s", filename);
			
			SendFile(IPAddress, port, new_filename);
		}        
        	else{			
			printf("Could not find file. Sending error to client.\n");
			char * notFound = "File not found!";
			send(new_fd, notFound, 100, 0);
		}
		ClearString(files, 500);
	}
	//Directory request
	else if(strcmp(command,"l") == 0){                  
		send(new_fd, good, strlen(good),0);
		printf("File list requested \n");
		printf("Sending file list to %s \n", IPAddress);
		char** files;
		files = CreateString(500);
		int numFiles;
		numFiles = GetFiles(files);
		SendDirectory(IPAddress, port, files, numFiles);
		ClearString(files,500);
	 }	
	else{
		send(new_fd, bad, strlen(bad), 0);
		printf("Invalid command.\n");
	}
	printf("Waiting for connections.\n");
}

/*************************************
 * WaitForConnection(int)
 * Waits for connection from a client
 * From Beegjs guide
 ************************************/
void WaitForConnection(int sockfd){	
	struct sockaddr_storage ClientAdd;	
    socklen_t AddSize;	
	int new_fd;
	
	while(1){		
		AddSize = sizeof(ClientAdd);		
		new_fd = accept(sockfd, (struct addrinfo *)&ClientAdd, &AddSize);
		
		if(new_fd == -1){			
			continue;
		}
		AcceptConnection(new_fd);
		close(new_fd);
	}
}

/*************************************
 * main(int, char*)
 * Begins server program
 ************************************/
int main(int argc, char *argv[]){
	//If number of arguments is wrong, send error to stderr
	if(argc != 2){                  
		fprintf(stderr, "Usage: ./ftserver [server]\n");
		exit(1);
	}
    
	printf("Server listening on port %s\n", argv[1]);   
	struct addrinfo* Socket = CreateAddress(argv[1]);
	int sockfd = CreateSocket(Socket);
	BindSocket(sockfd, Socket);
	ListenSocket(sockfd);
	WaitForConnection(sockfd);
	freeaddrinfo(Socket);
}