#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
//#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void *connection_handler(void *);

int main(int argc, char const* argv[])
{

	// create server socket
	int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    if (servSockD == -1){
        puts("Could not create socket");
    }

    // create client socket and sizeOf(struct sockaddr_in)
    int clientSocket, c, *newSock;

	// define server and client addresses
	struct sockaddr_in servAddr, cliAddr;

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(9013);
	servAddr.sin_addr.s_addr = INADDR_ANY;

	// bind socket to the specified IP and port
	if(bind(servSockD, (struct sockaddr*)&servAddr,sizeof(servAddr)) < 0) {
        puts("Bind failed");
        return 1;
    }

	// listen for connections, n = number of connections
	listen(servSockD, 3);

    puts("Waiting for incoming connections");
    c = sizeof(struct sockaddr_in);

	// integer to hold client socket                                   accept returns client socket!
	while (clientSocket = accept(servSockD, (struct sockaddr *)&cliAddr, (socklen_t*)&c))  {
                            //? э
        char *client_ip = inet_ntoa(cliAddr.sin_addr);
        int client_port = ntohs(cliAddr.sin_port);

        //wooooooooot ok now it works
        printf("[IP: %s; port: %i]Connection accepted\n", client_ip, client_port);

        pthread_t snifferThread;
        newSock = malloc(1);
        *newSock = clientSocket;

        if( pthread_create( &snifferThread , NULL ,  connection_handler , (void*) newSock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

        //???pthread_join( sniffer_thread , NULL);
        printf("[IP: %s; port: %i]Handler assigned\n", client_ip, client_port);

    }

    if (clientSocket < 0) {
        perror("Accept failed");
        return 1;
    }

	return 0;
}

void *connection_handler(void *sockD){
    int clientSocket = *(int*)sockD;

    //get time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // string store data to send to client
    char serMsg[255];
    sprintf(serMsg, "now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // send's messages to client socket
    send(clientSocket, serMsg, sizeof(serMsg), 0);

    // recive client message
    puts("Waitinng for client message");
    char strData[255];
    recv(clientSocket, strData, sizeof(strData), 0);
    printf("Client message: %s", strData);

    //unnecessaryWOOFCKNWHOREYOUDONTNEEDTHIS!!!
    //close(servSockD);
    free(sockD);
}
