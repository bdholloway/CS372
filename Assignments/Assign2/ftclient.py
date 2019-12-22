# Branden Holloway
# CS372, Project 2
# August 12th, 2019
#!/bin/python
import sys
from socket import *

# https://beej.us (Beej guide used as well as previous project 1)
# https://www.geeksforgeeks.org/socket-programming-python/
# https://docs.python.org/2/library/socket.html
# https://docs.python.org/3/howto/sockets.html
# https://docs.python.org/2/howto/sockets.html

##########################
#CreateSocket()
#Used to Create Socket thats being used
##########################
def CreateSocket():  
    #Checks number of arguments  
    if sys.argv[3] == "-l":
        numArguments = 4
    elif sys.argv[3] == "-g":                               
        numArguments = 5
    ServerPort = int(sys.argv[numArguments])
    ServerSocket = socket(AF_INET, SOCK_STREAM)
    ServerSocket.bind(('', ServerPort))
    ServerSocket.listen(1)
    DataSocket, address = ServerSocket.accept()
    return DataSocket

##########################
#ConnectServer()
#Connect server adn client 
##########################
def ConnectServer():    
    #Name of server, add the ending for ease of user                                           
    ServerName = sys.argv[1]+".engr.oregonstate.edu"        
    #Get Port Number
    ServerPort = int(sys.argv[2]) 
    #make socket                                     
    ClientSocket = socket(AF_INET,SOCK_STREAM)             
    ClientSocket.connect((ServerName, ServerPort))              
    return ClientSocket

##########################
#GetIP()
#Get the IP address
##########################
def GetIP():
    sock = socket(AF_INET, SOCK_DGRAM)    
    sock.connect(("8.8.8.8", 80))
    return sock.getsockname()[0]

##########################
#GetFile()
#Get File
##########################
def GetFile(dataSocket):   
    #Opens argument with "w" tag                
    file = open(sys.argv[4], "w")    
    buffer = dataSocket.recv(1000)
    
    while "__done__" not in buffer:
        #write to buffer till reaching keyword done
        file.write(buffer)
        buffer = dataSocket.recv(1000)
        
##########################
#GetList()
#Loops through till we hit the keyword "done"
##########################      
def GetList(dataSocket):
    FileName = dataSocket.recv(100)
    #While we havent reached "done", print FileName     
    while FileName != "done":        
        print FileName
        FileName = dataSocket.recv(100)


##########################
#GetData()
# Gets proper data wheather a file if -g or a list if -l
##########################
def GetData(clientSocket): 
    #checks argument, if its "-g" enter loop
    if sys.argv[3] == "-g":
        print "Reqesting file {}".format(sys.argv[4])  
        portnumber = 5
    #If not -g enter loop
    elif sys.argv[3] == "-l":
        print "Requesting list."
        portnumber = 4
    #Sends port number
    clientSocket.send(sys.argv[portnumber])    
    clientSocket.recv(1024)
    
    if sys.argv[3] == "-g":
        clientSocket.send("g")
    else:
        clientSocket.send("l")
    #Send IP address
    clientSocket.recv(1024)
    clientSocket.send(GetIP())                  
    response = clientSocket.recv(1024)
    
    #exit if receive bad response
    if response == "bad":                                                      
        print "Invalid Command Sent."
        exit(1)
    
    if sys.argv[3] == "-g":
        clientSocket.send(sys.argv[4])
        response = clientSocket.recv(1024)
        #if dont receive "File found" then something went wrong
        if response != "File found":                            
            print "Server did not find the file.'"
            return
    
    dataSocket = CreateSocket()
    #Get a file
    if(sys.argv[3] == "-g"):        
        GetFile(dataSocket)
    
    #get directory list
    elif sys.argv[3] == "-l":       
        GetList(dataSocket)

    #close when finished
    dataSocket.close()             




if __name__ == "__main__":  
    #Check argument number                                                                                                         #First we make sure user put the right number of arguments
    if len(sys.argv) < 5 or len(sys.argv) > 6:
        print "Invalid amount of arguments."
        exit(1)
    #Checks that argument is a valid flip server
    elif (sys.argv[1] != "flip1" and sys.argv[1] != "flip2" and sys.argv[1] != "flip3"):            
        print "Enter correct server name."
        exit(1)
    #Checks that port number is next argument and is in proper range
    elif (int(sys.argv[2]) < 1024 or int(sys.argv[2]) > 65535):                                
        print "Enter a port number in the range 1024-65535"
        exit(1)
    #Check that argument is either -g or -l
    elif (sys.argv[3] != "-g" and sys.argv[3] != "-l"):                     
        print "Invalid command. Must specify either -g or -l"
        exit(1)
    #Make sure port is correct    
    elif (sys.argv[3] == "-l" and (int(sys.argv[4])  < 1024 or int(sys.argv[4]) > 65535)):       
        print "Enter correct port number."
        exit(1) 
    elif (sys.argv[3] == "-g" and (int(sys.argv[5]) < 1024 or int(sys.argv[5]) > 65535)):        
        print "Enter correct port number."
        exit(1)
    #CreateSocket
    clientSocket = ConnectServer() 
    #Call get data
    GetData(clientSocket)           