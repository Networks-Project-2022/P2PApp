#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>

#include "PDU/PDU.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#define SERVER_PORT 10000 // Default port if none provided
#define NAMESIZE 20 // Max name size
#define FILENAME_BUFF_SIZE 100000 // Max filename size
#define MAXCON 200 // Max connections peer can have open

typedef struct entry {
  char usr[NAMESIZE];
  struct sockaddr_in addr;
  short token;
  struct entry *next;
} ENTRY;

typedef struct {
  char name[NAMESIZE];
  ENTRY *head;
} LIST;
LIST list[MAXCON];
int max_index = 0;

struct PDU tpdu;

void search(int, char *, struct sockaddr_in *);
void registration(int, char *, struct sockaddr_in *);
void deregistration(int, char *, struct sockaddr_in *);

int main(int argc, char *argv[]) {
  struct sockaddr_in sin, *p_addr; // From address for client
  ENTRY *p_entry;
  int s_port = SERVER_PORT; // Assign port number
  char name[NAMESIZE], usr[NAMESIZE];
  unsigned int alen = sizeof(struct sockaddr_in);
  int s, n, i, len, p_sock; // Initialize socket descriptor and type
  int pdulen = sizeof(struct PDU);
  struct hostent *hp;
  struct PDU rpdu;
  struct sockaddr_in fsin; // From address of a peer

  for (n = 0; n < MAXCON; n++) list[n].head = NULL;

  switch (argc) {
	case 1: {
	  break;
	}
	case 2: {
	  s_port = atoi(argv[1]); // NOLINT(cert-err34-c)
	  break;
	}
	default: {
	  printf("Usage: %s port\n", argv[0]);
	  exit(1);
	}
  }

  // Create UDP server
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons((u_short)s_port);

  // Allocate socket
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0) {
	printf("Could not create socket\n");
	exit(1);
  }

  // Bind socket
  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
	printf("Could not bind to port %d\n", s_port);
	exit(1);
  }
  printf("Index server started and listening on %d...\n", s_port);

  // Main loop
  ssize_t data;
  while (1) {
	if ((data = recvfrom(s, &rpdu, sizeof(rpdu), 0, (struct sockaddr *)&fsin, &alen)) < 0) {
	  printf("recvfrom error: %lu\n", data);
	} else {
	  switch (rpdu.type) {
		case REGISTER: {
		  break;
		}
		case CONTENT: {
		  break;
		}
		case DEREGISTER: {
		  break;
		}
	  }
	}
  }
  return 0;
}

void search(int s, char *data, struct sockaddr_in *addr) {
  /* Search content list and return the answer:
	 If found, send the address of the selected content server.
  */
}

void deregistration(int s, char *data, struct sockaddr_in *addr) {
  /* De-register the server of that content
  */
}

void registration(int s, char *data, struct sockaddr_in *addr) {
  /* Register the content and the server of the content
  */
}
