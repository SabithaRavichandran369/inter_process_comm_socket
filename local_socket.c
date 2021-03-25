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
#include <netinet/tcp.h>
#include <arpa/inet.h>


#define CHAT_SOCKET       "/tmp/chat.socket"
void report(const char* msg, int terminate) {
	perror(msg);
	if (terminate) exit(-1); /* failure */
}

int main() {

	struct stat statbuf;
	if (stat (CHAT_SOCKET, &statbuf) == 0) {
		if (unlink (CHAT_SOCKET) == -1)
			perror ("unlink");
	}

	int PortNumber=9090,ConversationLen=3,BuffSize=1024,MaxConnects=2;

	char data[BuffSize],ch;

	int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);          /* system picks underlying protocol (TCP) */
	if (fd < 0) report("socket", 1); /* terminate */

	/* bind the server's local address in memory */
	struct sockaddr_un socket_address;

	memset (&socket_address, 0, sizeof (struct sockaddr_un));
	socket_address.sun_family = AF_UNIX;
	strncpy (socket_address.sun_path, CHAT_SOCKET, sizeof(socket_address.sun_path) - 1);

	if (bind(fd,  (const struct sockaddr *) &socket_address, sizeof(struct sockaddr_un)) < 0)
		report("bind", 1); /* terminate */

	/* listen to the socket */
	if (listen(fd, MaxConnects) < 0) /* listen for clients, up to MaxConnects */
		report("listen", 1); /* terminate */
	fprintf(stderr, "Listening on port %i for clients...\npress q to quit\n", PortNumber);

	/* a server traditionally listens indefinitely */
	while (1) {

		int client_fd = accept(fd,	NULL, NULL);  /* accept blocks */
		if (client_fd < 0) {
			report("accept", 0); /* don't terminate, though there's a problem */
			continue;
		}
		/* read from client */
		while(1){
			char buffer[BuffSize + 1];
			memset(buffer, '\0', sizeof(buffer));
			int count = read(client_fd, buffer, sizeof(buffer));
			if (count > 0) {
				printf("client : ");
				puts(buffer);	
				printf("press y to send message [y/n/q] : ");
				scanf("\n%c",&ch);
				if(ch=='n')
					break;
				if(ch=='q')
				{
					write(client_fd,"q",sizeof("q"));
					return 0;
				}
				printf("server : ");
				scanf("\n%[^\n]s",data);
				write(client_fd,data,sizeof(data));
			}
		}
		close(client_fd); /* break connection */
	}  /* while(1) */
	return 0;
}
