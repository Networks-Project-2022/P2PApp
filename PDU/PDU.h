//
// Created by Michael Caira on 2022-11-09.
//

#ifndef P2PAPP_PDU_H
#define P2PAPP_PDU_H

#ifndef PDUH
#define PDUH

#define PDU_SIZE 101
#define DATA_SIZE 100
struct PDU {
  char type;
  char data[DATA_SIZE];
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

void sendPDU(char *buf, struct PDU pdu);

struct PDU receivePDU(char *data);

struct PDU createPDU(enum PDU_TYPE type, char *data);

#endif

#endif //P2PAPP_PDU_H
