CS372 Project 1 README.txt
Brian Metzger (metzgerb@oregonstate.edu)
Created: 2019-05-01
Last Modified: 2019-05-03 

INSTRUCTIONS:
1. compile chatclient and add executable permission to chatserve.py using the provided makefile
	(run command "make" to create both or "make chatclient" for chatclient or "make chatserve" for chatserve)
2. run chatserve.py using command: ./chatserve.py [portnum]
	(portnum is the port you wish to run the server on)
3. run chatclient using command: ./chatclient [host] [portnum]
	(I used flip3.engr.oregonstate.edu 23456 for my host and port number during testing. I also tested chatclient on flip2 while chatserve was on flip3)
4. chatclient will prompt user for a handle and then allow the user to input a message to send
5. chatserve will receive the message and allow the server user to respond with their on message
6. chatclient will receive the response and allow the user to input a message again
	(steps 5-6 repeat until on of the users enters the message "\quit")
	
To QUIT:
1. chatclient or chatserve must send the message "\quit" to close the connection
2. chatserve will continue running to accept a new connection
3. chatclient will close and return to the terminal prompt

NOTE:
"make clean" will remove the chatclient executable from the directory and remove the executable permission from chatserve.py
"make cclient" will remove the chatclient executable from the directory
"make cserve" will remove the executable permission from chatserve.py
