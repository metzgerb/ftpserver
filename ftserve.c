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

#define BUFFER_SIZE 503
#define MAX_BUFFER 700
#define SENTINEL "@!@"

//function declarations
void error(const char *msg);
int setupServer(int portNumber);
int sendMsg(int socketPtr);
int recvMsg(int socketPtr);


int main(int argc, char *argv[])
{
	int socketFD, serverConn;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in clientAddress;

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

	//loop while accepting clients
	while (1)
	{
		//begin listening on socket (up to 5 concurrent)
		listen(socketFD, 5);

		//accept a connection
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		serverConn = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		
		//check for accept error
		if (serverConn < 0)
		{
			error("ERROR on accept");
		}

		printf("Successful connection!\n");

		close(serverConn); // Close the existing socket which is connected to the client
		
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
	struct hostent* serverHostInfo;
	char serverName[256];

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
 * Function name: sendMsg
 * Inputs: Takes the socket to send to and a message to send
 * Outputs: Returns an integer
 * Description: The function sends a message to the client. It is generic and
 *		can be used to send any type of message
 ******************************************************************************/
int sendMsg(int socketPtr)
{
	char buffer[BUFFER_SIZE];
	char message[MAX_BUFFER];
	int charsWritten;
	
	// Get input message from user
	printf("> ");
	fflush(stdout);
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	//check if '\quit' command received
	if (strcmp(buffer, "\\quit") == 0)
	{
		//Add sentinel before sending quit to server
		strcat(buffer, SENTINEL);
		//stop client, send  move to close connection
		charsWritten = send(socketPtr, buffer, strlen(buffer), 0); // Write to the server
		
		//check for errors
		if (charsWritten < 0)
		{
			error("# CLIENT: ERROR writing to socket");
		}
		if (charsWritten < strlen(buffer)) //I chose to not loop while sending since the message is small. This will catch any error caused by that choice
		{
			printf("# CLIENT: WARNING: Not all data written to socket!\n");
		}

		return 1;
	}

	//prepend handle to message
	memset(message, '\0', sizeof(message)); // Clear out the message array
	strcpy(message, "> ");
	strcat(message, buffer);
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
			error("# CLIENT: ERROR writing to socket");
		}
		sendPtr += s;
		length -= s;
	}

	return 0;
}


/******************************************************************************
 * Function name: recvMsg
 * Inputs: Takes the socket to receive from
 * Outputs: Returns an integer
 * Description: The function receives a message from the client and parses it
 *		to determine what course of action to take
 ******************************************************************************/
int recvMsg(int socketPtr)
{
	char buffer[BUFFER_SIZE];
	char message[MAX_BUFFER];
	int charsRead;

	memset(buffer, '\0', sizeof(buffer));
	memset(message, '\0', sizeof(message));

	// Get return message from server
	while (strstr(buffer, SENTINEL) == NULL)
	{
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketPtr, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
		
		//check for socket read error
		if (charsRead < 0)
		{
			error("# CLIENT: ERROR reading from socket");
		}

		//add new characters
		strcat(message, buffer);
	}

	//strip term sentinel from return message
	message[strlen(message) - strlen(SENTINEL)] = '\0';

	//check for quit message from server
	if (strcmp(message, "\\quit") == 0)
	{
		//return quit command
		return 1;
	}

	printf("%s\n", message);
	fflush(stdout);

	return 0;
}