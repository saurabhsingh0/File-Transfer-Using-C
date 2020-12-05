# File-Transfer-Using-C

TigerC.c --> C program for Client
TigerS.c --> C program for Server(Kept is server folder)

TigerC --> executable file for Client
TigerS --> executable file for Server

For compiling use
"gcc TigerC.c -o TigerC" --> for client
"gcc TigerS.c -o TigerS" --> for server

In server folder there is usesr.txt
User along with their password is saved there
User auth is done from that file by matching the username and password

Start the server by: "./TigerS" in the server folder
Server is ready for connection

Start the client by: "./TigerC" from the path where client executable file is there

insert the command
tconnect 127.0.0.1 test test

for correct user and password : the client is connected
now you can enter
tget filename or tput filename and server reponds accordingly

3 bash commands

down.sh ---> create 100 downN.txt files N resresents number (run in server folder)
upload.sh ---> create 100 uploadN.txt files N resresents number (run in client path)
test.sh --> test behaviour (run in client path)
