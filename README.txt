CS372 Project 2 README.txt
Brian Metzger (metzgerb@oregonstate.edu)
Created: 2019-05-18
Last Modified: 2019-05-27

INSTRUCTIONS:
1. compile ftserve and add executable permission to ftclient.py using the provided makefile
	(run command "make" to create both or "make ftclient" for ftclient or "make ftserve" for ftserve)
2. run ftserve using command: ./ftserve [portnum]
	(I used flip2.engr.oregonstate.edu 54333 for my host and port number during testing. I also tested ftclient on flip3 while ftserve was on flip2)
3. run ftclient.py using command: ./ftclient.py [host] [control portnum] [data_port] [command] [optional file name]
4. use command '-l' in ftclient to get the directory listing from ftserve
5. use command '-g' with a file name to download a file from ftserve
	a. ftserve will return an error if the file does not exist
	b. ftserve will send the file if the file is found
5. when ftclient receives a file it will check that the file doesn't already exist before saving
	a. if the file does not exist, it will use the original name of the requested file
	b. if a file does exist, ftclient will prepend 'copy(#) - ' to the beginning of the file name
6. ftserve and ftclient will both close connections when they are not in use
7. ftserve will continue waiting for new connections after a connection is closed
	
NOTE:
"make clean" will remove the ftserve executable from the directory and remove the executable permission from ftclient.py
EXTRA CREDIT OPPORTUNITY: ftserve and ftclient can transfer any type of file (not just text files - I tested this by transferring an image.png file)