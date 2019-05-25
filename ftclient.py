#!/usr/bin/env python3

"""Program Name: ftclient.py
Python Version: 3
Description: Runs as a simple file transfer client to connect to a server and
    list or download files from the server
Author: Brian Metzger (metzgerb@oregonstate.edu)
Course: CS372 (Spring 2019)
Created: 2019-05-18
Last Modified: 2019-05-25
"""

from socket import *
import sys

"""
Function Name: send_msg
Description: sends a message to the server
Inputs: takes a socket file descriptor, a message and a sentinel
Outputs: returns success code or quit code
"""
def send_msg(socket, sentinel, message):
    #send message to server
    message = message + sentinel
    socket.sendall(message.encode())
    return 0


"""
Function Name: recv_msg
Description: waits to receive a message from the server
Inputs: takes a socket file descriptor, and a sentinel
Outputs: returns received message
"""
def recv_msg(socket, sentinel):
    #set initial values to empty strings
    buffer = ""
    message = ""
    
    #call recv in a loop until sentinel is detected in compiled message
    while(sentinel not in message):
        buffer = socket.recv(1024).decode()
        message += buffer
        
    #remove sentinel from message
    message = message.replace(sentinel, "")
    
    return message

"""
Function Name: recv_file
Description: waits to receive a filesize from the server then receives that many bytes from the server
Inputs: takes a socket file descriptor
Outputs: returns received file data
"""
def recv_file(socket, sentinel):
    #receive filesize
    file_size = int(recv_msg(socket, sentinel).strip('\x00'))
    print("file size: %d" % file_size)
    #send confirmation
    send_msg(socket, sentinel, "1")
    
    file_data = b""
    
    #receive file until max number of bytes received
    while(sys.getsizeof(file_data) < file_size):
        file_data += socket.recv(1024)
    print("Size of data received: %d" % sys.getsizeof(file_data))
    print("%s" % file_data.decode())
    return file_data
    

"""
Function Name: save_file
Description: attempts to save file downloaded from server
Inputs: takes string containing the contents of the received file
Outputs: returns an integer indicating success or failure
"""
def save_file(file_data):
    return 0


"""
Function Name: connect_server
Description: attempts to establish a control connection to the server on the specified port
Inputs: takes the host name and port number of the server
Outputs: returns a socket file descriptor to be used in later steps
"""
def connect_server(server, port_number): 
    #set up client socket
    new_socket = socket(AF_INET, SOCK_STREAM)
    new_socket.connect((server, port_number))
    
    return new_socket

"""
Function Name: setup_server
Description: sets up a data connection server on the specified port
Inputs: takes the port number to listen on
Outputs: returns a socket connection file descriptor to be used in later steps
"""
def setup_server(port_number):    
    #set up server socket
    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.bind(('', port_number))
    
    #begin listening
    server_socket.listen(1)
        
    #accept connection
    connection_socket, addr = server_socket.accept()
    
    return connection_socket


"""
Function Name: main
Description: controls the main logic of the program
Inputs: takes remote server host, the connection port number and data port number
Outputs: returns nothing
"""
def main(server, control_port, data_port, command, file_name = ""):
    #define initial variables
    SENTINEL = "@!@"
    
    #connect to server on control connection
    control_socket = connect_server(server,control_port)
    
    #send command to control connection and receive response
    send_msg(control_socket, SENTINEL, command + file_name)
    response = recv_msg(control_socket, SENTINEL)
    
    if(response):
        #send port number for data connection to control connection
        send_msg(control_socket,SENTINEL, str(data_port))
    
        #set up data server
        data_socket = setup_server(data_port)

        #Check if command is for directory listing
        if command == "-l":
            #print command being used
            print("Receiving directory structure from %s:%d" % (server, data_port))
            
            #receive directory listing on data port
            dir_list = recv_msg(data_socket, SENTINEL)
            
            #print directory listing
            print(dir_list)
        
        #assume "get" command used"
        else:
            #receive control response about file errors (stripping out null terminator
            #source: https://stackoverflow.com/questions/26635315/conversion-of-null-terminated-string-to-int-in-python
            file_found = int(recv_msg(control_socket, SENTINEL).strip('\x00'))
            
            #check if file was found
            if file_found:
                #print command being used
                print("Receiving \"%s\" from %s:%d" % (file_name, server, data_port))
                
                #receive file on data port
                file_data = recv_file(data_socket, SENTINEL)
                
                #print confirmation of completed transfer
                print("File Transfer complete")
            
            else:
                #print error message
                print("%s:%d says FILE NOT FOUND" % (server, data_port))
                
        #close data socket
        data_socket.close()
        
    #close socket connection
    control_socket.close()

"""
Description: intial code that validates the commandline arguments used
    and calls the other functions to create the connections and save files
Inputs: takes 4 or 5 arguments (server name, control port number, data port number, command, [file name])
Outputs: returns nothing
"""
if __name__ == "__main__":
    #check total argument count
    if len(sys.argv) != 5 and len(sys.argv) != 6:
        print("USAGE: %s server control_port data_port -command [file_name]" % sys.argv[0])
    #check that control port number is actually a number
    elif not sys.argv[2].isdigit():
        print("syntax error: control_port must be a number")
    #check that data port is actually a number
    elif not sys.argv[3].isdigit():
        print("syntax error: data_port must be a number")
    elif sys.argv[2] == sys.argv[3]:
        print("syntax error: control_port and data_port cannot be the same port")
    #check that correct commands are used
    elif sys.argv[4] not in ["-g", "-l"]:
        print("syntax error: command not recognized. Only use '-l' or '-g'")
    #check that file name provided with get command
    elif len(sys.argv) == 5 and sys.argv[4] == "-g":
        print("syntax error: no file name provided with '-g' command")
    #everything is ok, call main function
    else:
        if len(sys.argv) == 5:
            main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4])
        else:
            main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4], sys.argv[5])