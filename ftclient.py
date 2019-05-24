#!/usr/bin/env python3

"""Program Name: ftclient.py
Python Version: 3
Description: Runs as a simple file transfer client to connect to a server and
    list or download files from the server
Author: Brian Metzger (metzgerb@oregonstate.edu)
Course: CS372 (Spring 2019)
Created: 2019-05-18
Last Modified: 2019-05-24
"""

from socket import *
import sys


"""
Function Name: send_msg
Description: sends a message (user prompted) to the server
Inputs: takes a socket file descriptor, and a sentinel
Outputs: returns success code or quit code
"""
def send_msg(socket, sentinel):
    #get input from user
    message = input()
            
    #send message back
    message = message + sentinel
    socket.sendall(message.encode())
    return 0

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
Inputs: takes a socket file descriptor, a message and a sentinel
Outputs: 
"""
def recv_msg(socket, sentinel, buffer_size):
    #set initial values to empty strings
    buffer = ""
    message = ""
    
    #call recv in a loop until sentinel is detected in compiled message
    while(sentinel not in message):
        buffer = socket.recv(buffer_size).decode()
        message += buffer
        
    #remove sentinel from message
    message = message.replace(sentinel, "")
    
    #check if socket has closed using length of recd message
    if message == "\\quit":
        return -1
    else:    
        #print message to server
        print(message)
        return message


"""
Function Name: save_file
Description: attempts to save file downloaded from server
Inputs: takes string containing the contents of the received file
Outputs: returns an integer indicating success or failure
"""
def save_file(contents):
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
def main(server, control_port, data_port):
    #define initial variables
    SENTINEL = "@!@"
    
    #connect to server on control connection
    control_socket = connect_server(server,control_port)
    print("Control connection successful")
    
    #send hostname for data connection to control connection
    send_msg(control_socket, SENTINEL, gethostname())
    print("sent: %s\n" % socket.gethostname())
    #send port number for data connection to control connection
    send_msg(control_socket,SENTINEL, str(data_port))
    print("sent: %s\n" % str(data_port))
    
    #set up data server
    data_socket = setup_server(data_port)
    print("Data connection successful")
    
    #send command to control connection
    #receive response to command on control connection
    #receive response/data on data connection
    #do something with response
    data_socket.close()
    control_socket.close()

"""
Description: intial code that validates the commandline arguments used
    and calls the other functions to create the connections and save files
Inputs: takes 3 arguments (server name, control port number, data port number)
Outputs: returns nothing
"""
if __name__ == "__main__":
    #check total argument count
    if len(sys.argv) != 4:
        print("USAGE: %s server control_port data_port" % sys.argv[0])
    #check that port number is actually a number
    elif not sys.argv[2].isdigit():
        print("syntax error: control_port must be a number")
    elif not sys.argv[3].isdigit():
        print("syntax error: data_port must be a number")
    #everything is ok, call main function
    else:
        main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))