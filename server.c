#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
//#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>


void *connection_handler(void *);

struct arg_struct {
    int clientSocket;
    struct sockaddr_in cliAddress;
};

int main(int argc, char const* argv[])
{
	// create server socket
	int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    if (servSockD == -1){
        puts("Could not create socket");
        return 1;
    }

    // create client socket and sizeOf(struct sockaddr_in)
    int clientSocket, c, *newSock;

	// define server and client addresses
	struct sockaddr_in servAddr, cliAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(9025);
	servAddr.sin_addr.s_addr = INADDR_ANY;

	// bind socket to the specified IP and port
	if(bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        puts("Bind failed!!!1");
        if (setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &servAddr, sizeof(servAddr)) < 0){
            puts("Reuse addr failed");
            return 1;
        }

        if (setsockopt(servSockD, SOL_SOCKET, SO_REUSEPORT, &servAddr, sizeof(servAddr)) < 0){
            puts("Reuse port failed");
            return 1;
        }

//        setsockopt(servSockD, IPPROTO_TCP, SO_REUSEADDR, &servAddr, sizeof(servAddr));

    }

	// listen for connections, n = number of connections
	listen(servSockD, 3);

    puts("Waiting for incoming connections");
    c = sizeof(struct sockaddr_in);

	// integer to hold client socket                                   accept returns client socket!
	while (clientSocket = accept(servSockD, (struct sockaddr *)&cliAddr, (socklen_t*)&c))  {
        puts("hui");
                            //? э
        char *client_ip = inet_ntoa(cliAddr.sin_addr);
        int client_port = ntohs(cliAddr.sin_port);
        struct arg_struct args;
        args.clientSocket = clientSocket;
        args.cliAddress = cliAddr;

        //wooooooooot ok now it works
        printf("[IP: %s; port: %i]Connection accepted\n", client_ip, client_port);

        pthread_t snifferThread;

        if( pthread_create( &snifferThread , NULL ,  connection_handler , (void*)&args) < 0)
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

void *connection_handler(void *arguments){
    struct arg_struct *args = (struct arg_struct *)arguments;
    int clientSocket = args->clientSocket;
    struct sockaddr_in cliAddr = args->cliAddress;

    char *client_ip = inet_ntoa(cliAddr.sin_addr);
    int client_port = ntohs(cliAddr.sin_port);

    printf("[IP: %s; port: %i]Entered handler\n", client_ip, client_port);

    //get time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // string store data to send to client
    char serMsg[255];
    sprintf(serMsg, "now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // send's messages to client socket
    send(clientSocket, serMsg, sizeof(serMsg), 0);

    // recive client message
    printf("[IP: %s; port: %i]Waitinng for client command\n", client_ip, client_port);
    char strData[255];
    recv(clientSocket, strData, sizeof(strData), 0);
//    int i=system (strData);
//    printf ("The value returned was: %d.\n",i);

    FILE *comm = popen(strData, "r");
    char buf[256], output[1024];
    while (fgets(buf, sizeof(buf), comm) != 0) {
        strcat(output, buf);
    }
    pclose(comm);


    send(clientSocket, output, sizeof(output), 0);

    printf("[IP: %s; port: %i]Client command: %s", client_ip, client_port, strData);

    //unnecessaryWOOFCKNWHOREYOUDONTNEEDTHIS!!!
    //close(servSockD);
}
