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
#include <dirent.h>

#include "PDU/PDU.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#define SERVER_PORT 10000 // Default port if none provided
#define NAMESIZE 10 // Max name size
#define FILENAME_BUFF_SIZE 1000 // Max filename size
#define MAXCON 200 // Max connections peer can have open

// Keep track of registered content
struct {
  int val;
  char name[FILENAME_BUFF_SIZE];
} table[MAXCON];

char usr[NAMESIZE];
char localnames[FILENAME_BUFF_SIZE];
char servernames[FILENAME_BUFF_SIZE];

const char *filehome = "./";

int s_sock, peer_port, fd, nfds, port;
fd_set rfds, afds;

// Function prototypes
void registration(int, char *);
int search_content(int, char *, struct PDU *);
int client_download(char *, struct PDU *);
void server_download(int);
void deregistration(int, char *);
void online_list(int);
void local_list();
void quit(int);
void handler();

int main(int argc, char *argv[]) {
  int s_port = SERVER_PORT;
  int n;
  int alen = sizeof(struct sockaddr_in);
  struct hostent *hp;
  struct sockaddr_in server;
  char c, *host, name[NAMESIZE];
  struct sigaction sa;

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
  memset(&server, 0, sizeof(server)); // clear server address
  server.sin_family = AF_INET;
  server.sin_port = htons(s_port);

  // Check to see if the host entry is available
  if ((hp = gethostbyname(host))) memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  else if (((server.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)) {
	printf("Can't get host entry\n");
	exit(1);
  }
  // Create socket for index server connection
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
  printf("Enter a peer name to register content under on the index server (20 character limit):\n");
  scanf("%s", usr);

  // Initialize SELECT struct and table struct, monitoring for changes
  FD_ZERO(&afds);
  FD_SET(s_sock, &afds); // Listen on index server socket
  FD_SET(0, &afds); // Listen on read descriptor
  nfds = 1;
  for (n = 0; n < MAXCON; n++) {
	table[n].val = -1;
  }

  // Signal handler
  sa.sa_handler = (void (*)(int))handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);

  // Main loop
  while (1) {
	printf("Command:\n");
	memcpy(&rfds, &afds, sizeof(rfds));
	if (select(nfds, &rfds, NULL, NULL, NULL) == -1) {
	  printf("Select error: %s\n", strerror(errno));
	  exit(1);
	}
	// Select successful and FD is set, read command from filename
	if (FD_ISSET(0, &rfds)) {
	  c = getchar();
	  switch (c) {
		// Register content
		case 'R': {
		  printf("Enter the name of the file you wish to register with the index server.\n");
		  char filename[FILENAME_BUFF_SIZE];
		  memset(filename, '\0', sizeof(filename));
		  scanf("%s", filename);
		  registration(s_sock, filename);
		  break;
		}
		  // List local content
		case 'L': {
		  local_list();
		  printf("%s\n", localnames);
		  break;
		}
		  // List content on index
		case 'O': {
		  printf("O\n");
		  break;
		}
		  // Download content
		case 'D': {
		  printf("D\n");
		  break;
		}
		  // Deregister content
		case 'T': {
		  printf("T\n");
		  break;
		}
		  // Quit
		case 'Q': {
		  quit(s_sock);
		  break;
		}
		  // Command options
		case '?': {
		  printf("R-Content Registration; T-Content Deregistration; L-List Local Content\n");
		  printf("D-Download Content; O-List all the On-line Content; Q-Quit\n\n");
		}
		default: {
		  break;
		}
	  }
	  c = 0;
	} else {
	  // Download content
	  server_download(s_sock);
	}
  }
  return 0;
}

void quit(int s_sock) {
  /* De-register all the registrations in the index server        */
  exit(0);
}

// List local content (code used from Lab 4 server program)
void local_list() {
  memset(localnames, 0, sizeof(localnames));
  DIR *directory;
  struct dirent *dir;
  directory = opendir(filehome);
  if (directory) {
	char fnamebuf[FILENAME_BUFF_SIZE];
	// Clear filename buffer
	memset(fnamebuf, 0, sizeof(fnamebuf));
	char *fname = fnamebuf;
	int j = 0;
	while ((dir = readdir(directory)) != NULL) {
	  j++;
	  printf("%s", dir->d_name);
	  // Skip first 2 lines

	  strcpy(fname, dir->d_name);
	  fname += strlen(dir->d_name);
	  *fname = '\n';
	  fname++;

	}
	// Null terminate buffer
	*fname = '\0';
	closedir(directory);
	strcpy(localnames, fnamebuf);
  }
}

void online_list(int s_sock) {
  /* Contact index server to acquire the list of content */
}

void server_download(int s_sock) {
  /* Respond to the download request from a peer  */
}

int search_content(int s_sock, char *name, struct PDU *rpdu) {
  /* Contact index server to search for the content
	 If the content is available, the index server will return
	 the IP address and port number of the content server.        */


}

int client_download(char *name, struct PDU *pdu) {
  /* Make TCP connection with the content server to initiate the
	 Download.    */

}

void deregistration(int s_sock, char *name) {
  /* Contact the index server to deregister a content registration;          Update nfds. */

}

/* Create a TCP socket for content download
                        <96> one socket per content;
           Register the content to the index server;
           Update nfds; */
void registration(int s_sock, char *name) {
  // Open and read file if it exists
  char filepath[FILENAME_BUFF_SIZE];
  memset(filepath, '\0', sizeof(filepath));
  strcpy(filepath, filehome);
  strcat(filepath, name);
  FILE *file = fopen(filepath, "rb");
  if (file == NULL) {
	printf("Error registering content - could not find or load file specified.\n");
	return;
  }
  fclose(file);

  // Create a new TCP socket for data
  int content_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (content_sd == -1) {
	printf("Could not create filename socket: %s\n", filepath);
  } else {
	// Bind to socket
	struct sockaddr_in content_server;
	bzero((char *)&content_server, sizeof(struct sockaddr_in));
	content_server.sin_family = AF_INET;
	content_server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(content_sd, (struct sockaddr *)&content_server, sizeof(content_server)) == -1) {
	  printf("Could not bind to socket - registration error\n");
	  close(content_sd);
	} else {
	  // Gather registration data
	  FD_SET(content_sd, &afds);
	  nfds++;
	  char registerData[DEFAULT_DATA_SIZE];
	  memset(registerData, '\0', sizeof(registerData));
	  char *rp = registerData;
	  strcpy(rp, usr);
	  rp += 10;
	  strcpy(rp, filepath);
	  rp += 10;

	  // Get port number and server address
	  struct sockaddr_in sin;
	  socklen_t len = sizeof(sin);
	  if (getsockname(content_sd, (struct sockaddr *)&sin, &len) == -1)
		printf("Could not get socket name");
	  else {
		// Send registration PDU to index server
		strcpy(rp, inet_ntoa(sin.sin_addr));
		rp += strlen(inet_ntoa(sin.sin_addr));
		char *port_string = malloc(128);
		snprintf(port_string, 128, ":%u", sin.sin_port);
		strcpy(rp, port_string);
		free(port_string);

		// Convert PDU to string to be sent
		sendPDU(s_sock, REGISTER, registerData, DEFAULT_DATA_SIZE, NULL);

		// Receive error or acknowledgement from index server
		char registerResponseBuf[DEFAULT_DATA_SIZE + 1];
		memset(registerResponseBuf, '\0', sizeof(registerResponseBuf));
		read(s_sock, &registerResponseBuf, sizeof(registerResponseBuf));
		struct PDU registerResponse = receivePDU(registerResponseBuf, DEFAULT_DATA_SIZE);
		switch (registerResponse.type) {
		  case ACK: {
			printf("Content registered successfully and now available online for peers.\n");
			break;
		  }
		  case ERROR: {
			printf("Error: %s\n", registerResponse.data);
			close(content_sd);
			nfds--;
			break;
		  }
		  default: printf("Error, unexpected response type from index server while registering.\n");
		}
	  }
	}
  }
}

void handler() {
  close(s_sock);
}

#pragma clang diagnostic pop
