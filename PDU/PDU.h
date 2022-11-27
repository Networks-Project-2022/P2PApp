#ifndef P2PAPP_PDU_H
#define P2PAPP_PDU_H

#ifndef PDUH
#define PDUH

#define DEFAULT_DATA_SIZE 100
#define MAX_PACKET_SIZE 1427

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

struct PDU createPDU(enum PDU_TYPE type, int length);
struct PDU receivePDU(char *data, int length);
void sendPDU(struct PDU pdu, char *buf, int length);

#endif

#endif //P2PAPP_PDU_H
