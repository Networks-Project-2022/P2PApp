#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include "PDU.h"
// Creates a PDU with dynamic data length and returns a pointer to it
struct PDU createPDU(enum PDU_TYPE type, int length) {
  struct PDU result;
  if (length > MAX_DATA_SIZE) {
	result.type = ERROR;
	printf("Passed invalid length to createPDU\n");
	return result;
  }
  result.data = (char *)calloc(length, sizeof(char));
  memset(result.data, '\0', length * sizeof(char));
  result.type = type;
  return result;
}

// Take array of characters and turn into PDU struct
struct PDU receivePDU(char *data, int length) {
  struct PDU result = createPDU(data[0], length - 1);
  for (int i = 0; i < length - 1; i++) {
	result.data[i] = data[i + 1];
  }
  return result;
}

// Send data buffer with given PDU type
void sendPDU(int s, enum PDU_TYPE type, const char *data, int dataLength, const struct sockaddr *addr) {
  char *buf = (char *)calloc(dataLength, sizeof(char));
  buf[0] = type;
  for (int i = 1; i < dataLength; i++) {
	buf[i] = data[i - 1];
  }
  if (addr == NULL) {
	// null for connected sockets
	send(s, buf, dataLength + 1, 0);
  } else {
	// not null for unconnected sockets
	sendto(s, buf, dataLength + 1, 0, (const struct sockaddr *)addr, sizeof(*addr));
  }
  free(buf);
}