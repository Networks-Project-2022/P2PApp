//
// Created by Michael Caira on 2022-11-09.
//

#ifndef P2PAPP_PDU_H
#define P2PAPP_PDU_H

#ifndef PDUH
#define PDUH

#define DEFAULT_DATA_SIZE 100
#define MAX_PACKET_SIZE 1460

// PDU struct with type and data fields
struct PDU {
  char type;
  // Pointer to dynamic length data
  char *data;
};

enum PDU_TYPE {
  REGISTER = 'R',
  DOWNLOAD = 'D',
  SEARCH = 'S',
  DEREGISTER = 'T',
  CONTENT = 'C',
  AVAILABLE_CONTENT = 'O',
  ACK = 'A',
  ERROR = 'E',
};

struct PDU createPDU(enum PDU_TYPE type, char *data, int size);

struct PDU receivePDU(const char *data);

void stringifyPDU(char *buf, struct PDU pdu);

#endif

#endif //P2PAPP_PDU_H
