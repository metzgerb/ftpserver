CS372 Project 2 README.txt
Brian Metzger (metzgerb@oregonstate.edu)
Created: 2019-05-18
Last Modified: 2019-05-18

INSTRUCTIONS:
1. compile ftserve and add executable permission to ftclient.py using the provided makefile
	(run command "make" to create both or "make ftclient" for ftclient or "make ftserve" for ftserve)
2. run ftserve using command: ./ftserve [host] [portnum]
	(I used flip3.engr.oregonstate.edu 23456 for my host and port number during testing. I also tested chatclient on flip2 while chatserve was on flip3)
3. run ftclient.py using command: ./ftclient.py [portnum]
	(portnum is the port you wish to run the server on)
4. chatclient will prompt user for a handle and then allow the user to input a message to send
5. chatserve will receive the message and allow the server user to respond with their on message
6. chatclient will receive the response and allow the user to input a message again
	(steps 5-6 repeat until on of the users enters the message "\quit")
	
To QUIT:
1. ftclient or ftserve must send the message "\quit" to close the connection
2. ftserve will continue running to accept a new connection
3. ftclient will close and return to the terminal prompt

NOTE:
"make clean" will remove the ftserve executable from the directory and remove the executable permission from ftclient.py
