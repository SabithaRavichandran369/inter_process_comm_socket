#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/stat.h>

#define CHAT_SOCKET       "/tmp/chat.socket"

void report(const char* msg, int terminate) {
	perror(msg);
	if (terminate) exit(-1); /* failure */
}

int main() {

	int PortNumber=9090,ConversationLen=3,BuffSize=1024;

	char data[BuffSize],ch;
	/* fd for the socket */

	int sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);           /* system picks protocol (TCP) */

	if (sockfd < 0) report("socket", 1); /* terminate */

	/* get the address of the host */
	struct sockaddr_un socket_address;

	memset (&socket_address, 0, sizeof (struct sockaddr_un));
	socket_address.sun_family = AF_UNIX;
	strncpy (socket_address.sun_path, CHAT_SOCKET, sizeof (socket_address.sun_path) - 1);
	/* port number in big-endian */

	if (connect(sockfd, (const struct sockaddr *) &socket_address, sizeof (struct sockaddr_un)) == -1)
		report("connect", 1);

	/* Write some stuff and read. */
	puts("Connect to server, about to write some stuff...");
	while(1){
		printf("press y to send message [y/n] : ");
		scanf("\n%c",&ch);
		if(ch=='n')
			break;
		printf("client : ");
		scanf("\n%[^\n]s",data);
		if (write(sockfd,data, sizeof(data)) > 0) {
			char buffer[BuffSize + 1];
			memset(buffer, '\0', sizeof(buffer));
			printf("server : ");
			if (read(sockfd, buffer, sizeof(buffer)) > 0)
			{       if(!strcmp("q",buffer))
				{ printf("server went off\n");
					break;}
				puts(buffer);
			}   }
	}
	puts("Client done, about to exit...");
	close(sockfd); /* close the connection */
	return 0;
}

