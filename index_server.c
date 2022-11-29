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
#define NAMESIZE 10 // Max name size
#define FILENAME_BUFF_SIZE 1000 // Max filename size
#define MAXCON 200 // Max connections peer can have open

// Structure defining a piece of content available on the index server
typedef struct entry {
  char filename[FILENAME_BUFF_SIZE];
  char addr[DEFAULT_DATA_SIZE];
  short count;
  struct entry *next;
  struct entry *prev;
} ENTRY;

// Structure defining the content of a single peer
typedef struct {
  char usr[NAMESIZE];
  ENTRY *head;
} LIST;

LIST list[MAXCON];

int max_index = 0;

void online_list();
void search(int, char *, struct sockaddr_in *);
void registration(int, const char *, struct sockaddr_in *);
void deregistration(int, char *, struct sockaddr_in *);

int main(int argc, char *argv[]) {
  struct sockaddr_in sin, *p_addr; // From address for client
  int s_port = SERVER_PORT; // Assign port number
  unsigned int alen = sizeof(struct sockaddr_in);
  int s, n, i, len, p_sock; // Initialize socket descriptor and type
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
	char rbuf[MAX_DATA_SIZE];
	// Send error PDU in case socket cannot receive data
	if ((data = recvfrom(s, &rbuf, sizeof(rbuf), 0, (struct sockaddr *)&fsin, &alen)) < 0) {
	  printf("recvfrom error: %lu\n", data);
	  char errMsg[] = "Check server status.\0";
	  sendPDU(s, ERROR, errMsg, sizeof(errMsg) / sizeof(char), (const struct sockaddr *)&fsin);
	} else {
	  struct PDU rpdu = receivePDU(rbuf, MAX_DATA_SIZE);
	  switch (rpdu.type) {
		case REGISTER: {
		  registration(s, rpdu.data, &fsin);
		  break;
		}
		case CONTENT: {
		  break;
		}
		  // List out content only, do not repeat content names
		case AVAILABLE_CONTENT: {
		  online_list();
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

void online_list() {

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

// Register the content and the server of the content
void registration(int s, const char *data, struct sockaddr_in *addr) {
  // Send back an error if max connections are reached.
  if (max_index == MAXCON) {
	char errMsg[] = "Max number of connections reached, try again later\0";
	sendPDU(s, ERROR, errMsg, sizeof(errMsg) / sizeof(char), (const struct sockaddr *)addr);
	return;
  }
  char peerName[NAMESIZE];
  char contentName[FILENAME_BUFF_SIZE];
  char address[DEFAULT_DATA_SIZE];
  for (int j = 0; j < 20; j++) {
	if (j < 10) peerName[j] = data[j];
	else {
	  contentName[j - 10] = data[j];
	}
  }
  int j = 20;
  while (data[j] != '\0') {
	address[j - 20] = data[j];
	j++;
  }
  int peerIndex = 0;
  int peerExists = 0;
  int contentExists = 0;
  for (j = 0; j < MAXCON; j++) {
	// Peer name exists, check to see if it already has the same content registered
	if (!strcmp(list[j].usr, peerName)) {
	  peerExists = 1;
	  peerIndex = j;

	  ENTRY *copy = malloc(sizeof(ENTRY));
	  strcpy(copy->filename, list[peerIndex].head->filename);
	  strcpy(copy->addr, list[peerIndex].head->addr);
	  copy->count = list[peerIndex].head->count;
	  ENTRY *copyHead = copy;
	  ENTRY *head = list[peerIndex].head;
	  while (head != NULL) {
		if (!(strcmp(head->filename, contentName))) {
		  contentExists = 1;
		  break;
		}
		copyHead->next = malloc(sizeof(ENTRY));
		strcpy(copyHead->filename, head->filename);
		strcpy(copyHead->addr, head->addr);
		copyHead->count = head->count;
		copyHead->next = head->next;
		head = head->next;
	  }
	  list[peerIndex].head = copy;
	}
  }
  // Add PDU to entry list, send back and ACK
  if (contentExists == 0) {
	ENTRY newContent;
	strcpy(newContent.filename, contentName);
	strcpy(newContent.addr, address);
	newContent.count = 0;
	newContent.next = NULL;
	// Create a new peer list, add content as head if non-existent
	if (peerExists == 0) {
	  LIST newPeer;
	  strcpy(newPeer.usr, peerName);
	  newPeer.head = &newContent;
	  list[max_index] = newPeer;
	  max_index++;
	} else {
	  // Add content to existing peer list if possible
	  ENTRY *copy = malloc(sizeof(ENTRY));
	  strcpy(copy->filename, list[peerIndex].head->filename);
	  strcpy(copy->addr, list[peerIndex].head->addr);
	  copy->count = list[peerIndex].head->count;
	  ENTRY *copyHead = copy;
	  ENTRY *contentTail = list[peerIndex].head;
	  while (contentTail != NULL) {
		copy->next = malloc(sizeof(ENTRY));
		strcpy(copyHead->filename, contentTail->filename);
		strcpy(copyHead->addr, contentTail->addr);
		copyHead->count = contentTail->count;
		copyHead->next = contentTail->next;
		contentTail = contentTail->next;
	  }
	  contentTail = &newContent;
	  copyHead->next = contentTail;
	  list[peerIndex].head = copy;
	}
	sendPDU(s, ACK, "\0", sizeof("\0") / sizeof(char), (const struct sockaddr *)addr);
  } else {
	// Send back an error if content already exists on peer
	char registerError[] = "Content name on that peer already exists.\0";
	sendPDU(s, ERROR, registerError, sizeof(registerError) / sizeof(char), (const struct sockaddr *)addr);
  }
}
