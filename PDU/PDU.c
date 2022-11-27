#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "PDU.h"
// Creates a PDU with dynamic data length and returns a pointer to it
struct PDU createPDU(enum PDU_TYPE type, int length) {
  struct PDU result;
  if (length > MAX_PACKET_SIZE) {
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
  strcpy(result.data, data + 1);
  return result;
}

// Break PDU into string
void sendPDU(struct PDU pdu, char *buf, int length) {
  buf[0] = pdu.type;
  for (int i = 0; i < length; i++) buf[i + 1] = pdu.data[i];
}
