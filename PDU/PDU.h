#ifndef P2PAPP_PDU_H
#define P2PAPP_PDU_H

#ifndef PDUH
#define PDUH

#include <sys/socket.h>

#define DEFAULT_DATA_SIZE 100
#define MAX_DATA_SIZE 1426

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
void sendPDU(int s, enum PDU_TYPE type, const char *data, int dataLength, const struct sockaddr *addr);

#endif

#endif //P2PAPP_PDU_H
