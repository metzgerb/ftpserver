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
Description: sends a message to the server
Inputs: takes a socket file descriptor, a message and a sentinel
Outputs: returns success code or quit code
"""
def send_msg(socket, sentinel):
    #get input from user
    message = input()
            
    #check if message is "\quit"
    if message == "\\quit":
        #send quit message back with sentinel
        message += sentinel
        socket.sendall(message.encode())
        return -1          
    else: 
        #send message back
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
    #initialize values
    #MAX_BUFFER = 700
    #SENTINEL = "@!@"
    
    #set up client socket
    new_socket = socket(AF_INET, SOCK_STREAM)
    new_socket.connect((server, port_number))
    
    #continuously receive and send messages
    #while True:
        #receive a message from client
        #recd_message = recv_msg(connection_socket, SENTINEL, 700)
            
        #check if received message was quit command
        #if recd_message == -1:
        #    break
        
        #send message to server
        #send_message = send_msg(new_socket, SENTINEL)
            
        #check if sent message was quit command
        #if send_message == -1:
            #break
    #print("Connection successful")
     
    #close connection
    #new_socket.close()
    return new_socket

def main(server, control_port, data_port):
    #connect to server
    control_socket = connect_server(server,control_port)
    print("Connection successful")
    #if successful, set up data server
    control_socket.close()
    #send port number for data connection to control connection
    #send command to control connection
    #receive response to command on control connection
    #receive response/data on data connection
    #do something with response

"""
Description: main code that validates the commandline arguments used
    and calls the other functions to create the connections and save files
Inputs: takes X arguments (server name, control port number, data port number)
Outputs: possibly downloads files from server
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
        #connect_server(sys.argv[1], int(sys.argv[2]))
        main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))