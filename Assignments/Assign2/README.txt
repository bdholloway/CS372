Comments:
    1. ftclient.py and ftserver.c must be in seperate directories to work properly
    2. Compile and run server, then client
    3. If port does not work, try a new one, it make be already in use

Running Server:
    1. To compile ftserver.c type the following command into the prompt
        gcc -o ftserver ftserver.c
    2. To begin server type the following
        ./ftserver [port#] (port# being the port you want to run the server on) 

Running Client:
    1. Give it excecute permission by typing the following command in the prompt
        chmod +x ftclient.py
    2. Type the following to transfer file
        a. To get file
            python ftclient.py [server name] [server port#] -g [filename] [data port#] 
            (server name being either flip1, flip2, or flip3, which ever server is on)  
        b. To get list
            python ftclient.py [server name] [server port#] -l [data port#] 
            (server name being either flip1, flip2, or flip3, which ever server is on)
Example:
    server:
    gcc -o ftserver ftserver.c
    ./ftserver 30020
    
    client:
    chmod +x ftclient.py
    python ftclient.py flip1 30020 -g test.txt 30021


