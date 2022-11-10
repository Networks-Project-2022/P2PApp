//
// Created by Michael Caira on 2022-11-09.
//
#include "PDU.h"

// Creates a PDU with dynamic data size and returns a pointer to it
struct PDU createPDU(enum PDU_TYPE type, char *data) {
  struct PDU result;
  return result;
}

// Create a PDU based on data received over socket
struct PDU receivePDU(char *data) {
  struct PDU result;
  return result;
}

// Writes contents of the PDU to the buffer passed in to be sent over socket
void sendPDU(char *buf, struct PDU pdu) {}

