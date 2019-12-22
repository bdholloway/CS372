#Complile and Run both chatserver.py and chatclient.c on flip1 server
IMPORTANT:
1. Can be run on flip1,2,3 as long as both terminal windows are on the same flip
2. Be sure the port # is the same for both, if socket fails try new port #
3. Must bring up server first, then client
4. Client makes first message, then alternates each message

Run in server terminal
1. python chatserver.py <port#>
2. enter valid username
3. wait for client message

Run in client terminal
1. $gcc -o chatclient chatclient.c
2. $./chatclient flipX.engr.oregonstate.edu <port#>
3. enter valid username
4. begin chat by messaging from client first

Extra Credit:
1. Added Color feature to both chats to make it easier to see who is who
