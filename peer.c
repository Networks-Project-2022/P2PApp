#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "PDU/PDU.h"

#define SERVER_PORT 10000 // Default port if none provided
#define NAMESIZE 20 // Max length of peer name
#define MAXCON 200 // Maximum connections peer can have open

char username[NAMESIZE];
int s_sock, peer_port, fd, nfds;
fd_set rfds, afds;

int main(int argc, char **argv) {
  int s_port = SERVER_PORT;
  int n;
  int alen = sizeof(struct sockaddr_in);
  struct hostent *hp;
  struct sockaddr_in server;
  char c, *host, name;

  switch (argc) {
	case 2: {
	  host = argv[1];
	  break;
	}
	case 3: {
	  host = argv[1];
	  s_port = atoi(argv[2]); // NOLINT(cert-err34-c)
	  break;
	}
	default: {
	  printf("Usage: %s host [port]\n", argv[0]);
	  exit(1);
	}
  }

  // Establish UDP connection with the index server
  memset(&server, 0, alen); // clear server address
  server.sin_family = AF_INET;
  server.sin_port = htons(s_port);

  // Check to see if the host entry is available
  if ((hp = gethostbyname(host))) memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  else if (((server.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)) {
	printf("Can't get host entry\n");
	exit(1);
  }
  // Create socket for index server
  s_sock = socket(PF_INET, SOCK_DGRAM, 0);
  if (s_sock < 0) {
	printf("Can't create socket\n");
	exit(1);
  }
  if (connect(s_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
	printf("Can't connect to socket\n");
	exit(1);
  }

  // Socket created successfully, enter name to register with index server
  printf("Enter a username to register with the index server (20 character limit)\n");
  scanf("%s", username);
}

void handler() {
  close(s_sock);
}