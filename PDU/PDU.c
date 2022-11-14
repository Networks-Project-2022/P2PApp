//
// Created by Michael Caira on 2022-11-09.
//
#include <stdlib.h>
#include "PDU.h"
const int MAX_PACKET_SIZE = 1460;
// Creates a PDU with dynamic data size and returns a pointer to it
struct PDU createPDU(enum PDU_TYPE type, char *data, int size) {
  struct PDU result;
  if (size > MAX_PACKET_SIZE) {
	return result;
  }
  char new_data[DEFAULT_DATA_SIZE];
  if (type == CONTENT) {
	result.data = (char *)calloc(size, sizeof(char));
  } else {
	result.data = new_data;
  }
  result.type = type;
  return result;
}

// Create a PDU based on data received over socket
struct PDU receivePDU(const char *data) {
  struct PDU result;
  return result;
}

// Writes contents of the PDU to the buffer passed in to be sent over socket
void stringifyPDU(char *buf, struct PDU pdu) {}

