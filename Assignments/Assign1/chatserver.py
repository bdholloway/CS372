#!/bin/python
import sys
from socket import *

#https://docs.python.org/2/howto/sockets.html
#https://docs.python.org/release/2.6.5/library/internet.html

############################
# GetUserName()
# Gets UserName
############################
def GetUserName(UserName):
    UserName = raw_input("Enter a valid username up to 8 characters long >")
    return UserName

############################
# Chat()
# Sends and Receives Messages
############################
def Chat(Connection_Socket, Client, UserName):
               
    while 1:                
        #Input Portion
        InputMsg = Connection_Socket.recv(501)[0:-1]        
        if InputMsg == "":                  
            print "Waiting For Connection"
            break
        print "\033[92m{}> {}\033[00m".format(Client, InputMsg)       
        
        #Output
        OutputMsg = ""
        while len(OutputMsg) > 500 or len(OutputMsg) == 0:
            OutputMsg = raw_input("{}> ".format(UserName))    
        if OutputMsg == "\quit":              
            print "Waiting......."
            break
        Connection_Socket.send(OutputMsg)

############################
# HandShake()
# Connects
############################
def HandShake(connection, UserName):
    Client = connection.recv(1024)    
    connection.send(UserName)
    return Client

if __name__ == "__main__":    
    if len(sys.argv) != 2:             
        print "Invalid Port Number"
        exit(1)
   
    PortNum = sys.argv[1]
    NewSock = socket(AF_INET, SOCK_STREAM)         
    NewSock.bind(('', int(PortNum)))           
    NewSock.listen(1)         
    UserName = ""
    #Checks user name length
    while len(UserName) > 8 or len(UserName) == 0:     
        UserName = GetUserName(UserName)
        print "^^^^^Server is now live^^^^^^"
        print "Awaiting message..."
    while 1:
        connection, address = NewSock.accept()                         
        print "Connected on address {}".format(address)
        
        Chat(connection, HandShake(connection, UserName), UserName)
        
        connection.close()