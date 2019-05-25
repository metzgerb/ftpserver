/******************************************************************************
* Program Name: ftserve.c
* Description: Runs as a server to connect to ftclient and send requested files
* Sources: Base code used from CS344 server.c file (winter 2019 term) and as
		noted within code.
* Author: Brian Metzger (metzgerb@oregonstate.edu)
* Course: CS372 (Spring 2019)
* Created: 2019-05-18
* Last Modified: 2019-05-24
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h> //for directory listing

#define BUFFER_SIZE 256
#define MAX_BUFFER 1024
#define SENTINEL "@!@"

//function declarations
void error(const char *msg);
int setupServer(int portNumber);
int connectServer(char* server, int portNumber);
int sendMsg(int socketPtr, char* buffer);
int recvMsg(int socketPtr, char** message);
void parseCmd(int socketPtr, char* client, char* service, char* message);
void getDir(char** result);

int main(int argc, char *argv[])
{
	int socketFD, controlConn;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in clientAddress;

	//char command[MAX_BUFFER];
	char* command;
	char client[MAX_BUFFER];
	char service[20];
	
	//check for correct number of arguments
	if (argc != 2) 
	{ 
		fprintf(stderr,"USAGE: %s port\n", argv[0]); 
		exit(0); 
	}

	//create socket and connect to server
	socketFD = setupServer(atoi(argv[1]));

	//check for socket/connection errors
	if (socketFD == -1)
	{
		error("ERROR opening socket");
	}
	else if (socketFD == -2)
	{
		error("ERROR binding on port");
	}
	else if (socketFD == -3)
	{
		error("ERROR No such host");
	}

	printf("Server open on %s\n", argv[1]);

	//loop while accepting clients
	while (1)
	{
		//begin listening on socket (up to 5 concurrent)
		listen(socketFD, 5);
		
		//accept a connection
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		controlConn = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		
		//check for accept error
		if (controlConn < 0)
		{
			error("ERROR on accept");
		}

		//parse and output client name (Source: https://beej.us/guide/bgnet/html/multi/getnameinfoman.html)
		getnameinfo(&clientAddress, sizeOfClientInfo, client, sizeof(client), service, sizeof(service), 0);
		printf("Connection from %s:%s\n", client, service);
		
		//receive command for data connection
		recvMsg(controlConn, &command);

		//parse command and respond to client
		parseCmd(controlConn, client, service, command);

		//free command memory
		free(command);

		//close connection
		printf("Closing client connection\n\n");
		close(controlConn); // Close the existing socket which is connected to the client
		
	}

	close(socketFD); // Close the socket

	return 0;
}


/******************************************************************************
 * Function name: error
 * Inputs: Takes a message
 * Outputs: Returns nothing
 * Description: The function prints the error and exits the program.
 ******************************************************************************/
void error(const char *msg) 
{ 
	perror(msg); 
	exit(0); 
}


/******************************************************************************
 * Function name: setupServer
 * Inputs: Takes port number to listen on
 * Outputs: Returns socket file descriptor for server
 * Description: The function sets up a server to be ready to listen on a 
 *		specific port.
 ******************************************************************************/
int setupServer(int portNumber)
{
	int socketPtr;
	struct sockaddr_in serverAddress;

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Convert the machine name into a special form of address
	
	// Set up the socket
	socketPtr = socket(AF_INET, SOCK_STREAM, 0); // Create the socket

	//check that socket was opened successfully
	if (socketPtr < 0)
	{
		return -1;
	}

	// Connect to server and check that connection was successful
	if (bind(socketPtr, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))
	{
		return -2;
	}
	
	return socketPtr;
}


/******************************************************************************
 * Function name: connectServer
 * Inputs: Takes the server and port number of the server
 * Outputs: Returns a socket if connected successfully
 * Description: The function attempts to connect to the specified server and
		port using a TCP connection. If socket fails to open, function return -1
		if socket fails to connect to server, function returns -2.
 ******************************************************************************/
int connectServer(char* server, int portNumber)
{
	int socketPtr;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(server); // Convert the machine name into a special form of address

	//check if server info could not be obtained
	if (serverHostInfo == NULL)
	{
		error("ERROR, no such host\n");
	}

	// Copy in the address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr_list[0], serverHostInfo->h_length);

	// Set up the socket
	socketPtr = socket(AF_INET, SOCK_STREAM, 0); // Create the socket

	//check that socket was opened successfully
	if (socketPtr < 0)
	{
		return -1;
	}

	// Connect to server and check that connection was successful
	if (connect(socketPtr, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		return -2;
	}

	return socketPtr;
}


/******************************************************************************
 * Function name: sendMsg
 * Inputs: Takes the socket to send to and a message to send
 * Outputs: Returns an integer
 * Description: The function sends a message to the client. It is generic and
 *		can be used to send any type of message
 ******************************************************************************/
int sendMsg(int socketPtr, char* buffer)
{
	char* message = malloc((strlen(buffer) + strlen(SENTINEL) + 1 ) * sizeof(char));
	memset(message, sizeof(message), '\0');

	//copy sentinel to message
	strcpy(message, buffer);
	strcat(message, SENTINEL);

	// Send message to server
	long length = strlen(message) + 1;
	char* sendPtr = message;

	//loop and send message until all is sent
	//modified from source: https://stackoverflow.com/questions/13479760/c-socket-recv-and-send-all-data
	while (length > 0)
	{
		long s = send(socketPtr, sendPtr, length, 0);

		//check for write error
		if (s < 0)
		{
			free(message);
			error("ERROR writing to socket");
		}
		sendPtr += s;
		length -= s;
	}

	free(message);
	return 0;
}


/******************************************************************************
 * Function name: recvMsg
 * Inputs: Takes the socket to receive from
 * Outputs: Returns an integer
 * Description: The function receives a message from the client and parses it
 *		to determine what course of action to take
 ******************************************************************************/
int recvMsg(int socketPtr, char** message)
{
	//set initial string length, capacity and allocate memory
	int length = 0;
	int capacity = 10;
	*message = malloc(capacity * sizeof(char));
	memset(*message, capacity, '\0');
	strcpy(*message, "");
	
	int charsRead;
	char buffer[BUFFER_SIZE];
	memset(buffer, '\0', sizeof(buffer));

	// Get return message from server
	while (strstr(buffer, SENTINEL) == NULL)
	{
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketPtr, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end

		//check for socket read error
		if (charsRead < 0)
		{
			free(*message);
			error("ERROR reading from socket");
		}

		//update expected length
		length += charsRead + 1;

		//check if resize is necessary
		if (length > capacity - 1)
		{
			//double capacity
			capacity *= 2;
			//reallocate memory
			*message = (char *)realloc(*message, capacity);
		}

		//add new characters
		strcat(*message, buffer);
	}

	//strip term sentinel from return message
	(*message)[strlen(*message) - strlen(SENTINEL)] = '\0';

	return 0;
}


/******************************************************************************
 * Function name: parseCmd
 * Inputs: Takes the control socket, client hostname, service and command message
 * Outputs: returns nothing
 * Description: The function parses the command received from the client and 
 *		either sends a directory listing, sends a file, or send an error.
 ******************************************************************************/
void parseCmd(int socketPtr, char* client, char* service, char* message)
{
	int dataConn;
	char* dataPort;
	char command[3];
	
	//separate command from file name
	memcpy(command, message, 2);
	command[2] = '\0';

	//parse command and send command confirmation
	if (strcmp(command, "-l") == 0 || strcmp(command, "-g") == 0)
	{
		//send good command confirmation
		sendMsg(socketPtr, "1");

		//receive port number for data connection
		recvMsg(socketPtr, &dataPort);

		//connect to client on data port
		dataConn = connectServer(client, atoi(dataPort));
		
		//check if command is for directory listing
		if (strcmp(command, "-l") == 0)
		{
			//print command requested
			printf("List directory requested on port %s\n", dataPort);
			
			//store directory listing
			char* dirList;
			getDir(&dirList);

			//TODO: send directory listing on data connection
			printf("Sending directory contents to %s:%s\n", client, dataPort);
			//sendMsg(dataConn, dirList);

			//TODO: free allocated string
			free(dirList);
		}
		else //assume "get" command issued
		{
			//parse file name
			char fileName[256];
			strcpy(fileName, &message[2]);
			fileName[255] = '\0'; //for safety
			printf("File \"%s\" requested on port %s\n", fileName, dataPort);

			//TODO: check if file found, send if found, else send error
			if (0)
			{
				//print file found confirmation
				printf("Sending \"%s\" to %s:%s\n", fileName, client, dataPort);

				//send file found confirmation to control connection
				sendMsg(socketPtr, "1");

				//send file through data connection

			}
			else //file was not found
			{
				//print error
				printf("File not found. Sending error message to %s:%s\n", client, service);

				//send error to control connection
				sendMsg(socketPtr, "0");
			}
		}

		//close data connection
		free(dataPort);
		close(dataConn);
	}
	else //catchall for invalid commmands
	{
		//send error code
		printf("Error parsing command\n");
		sendMsg(socketPtr, "0");
	}
}

/******************************************************************************
 * Function name: getDir
 * Inputs: pointer to an empty dynamically allocated char *
 * Outputs: returns nothing
 * Description: The function attempts to open and read the current directory and
 *		adds its contents to the char *
 * Source: https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
 ******************************************************************************/
void getDir(char** result)
{
	struct dirent *de;  // Pointer for directory entry 

	//set initial string length, capacity and allocate memory
	int length = 0;
	int capacity = 10;
	*result = malloc(10 * sizeof(char));
	memset(*result, capacity, '\0');
	strcpy(*result,"");

	// opendir() returns a pointer of DIR type.  
	DIR *dr = opendir(".");

	if (dr == NULL)  // opendir returns NULL if couldn't open directory 
	{
		error("Could not open current directory");
	}

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html 
	// for readdir() 
	while ((de = readdir(dr)) != NULL)
	{
		//update expected length
		length += strlen(de->d_name)+1;
		
		//check if resize is necessary
		if (length > capacity - 1)
		{
			//double capacity
			capacity *= 2;
			//reallocate memory
			*result = (char *)realloc(*result, capacity);
		}

		//add dir entry to result string
		strcat(*result, de->d_name);
		strcat(*result, "\n");
	}
	
	//remove final newline
	(*result)[strlen(*result)-1] = '\0';

	closedir(dr);
}