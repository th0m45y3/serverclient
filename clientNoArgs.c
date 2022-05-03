#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
//#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
//#include <sys/types.h>
#include <unistd.h>

int main(int argc, char const* argv[])
{
	int sockD = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servAddr;

	servAddr.sin_family = AF_INET;
	servAddr.sin_port
		= htons(9013); // use some unused port number
	servAddr.sin_addr.s_addr = INADDR_ANY;

	if (connect(sockD, (struct sockaddr*)&servAddr,
                sizeof(servAddr)) < 0) {
		printf("Error...\n");
	}

	else {
		char strData[255], cliMsg[255];

		recv(sockD, strData, sizeof(strData), 0);

		printf("Message: %s\nEnter your message:", strData);
        fgets(cliMsg, sizeof cliMsg, stdin);

        send(sockD, cliMsg, sizeof(cliMsg), 0);
	}

    //unnecessary
    close(sockD);

	return 0;
}
