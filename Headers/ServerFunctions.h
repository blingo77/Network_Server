#ifndef  SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <WinSock2.h>

extern int load_WSA_dll(void);
extern SOCKET socket(void);
extern int bindSocket(int port, SOCKET serverSocket);
extern int listen(SOCKET serverSocket);
extern SOCKET acceptSocket(SOCKET serverSocket);
int receiveData(SOCKET acceptedSocket);

#endif // ! SERVER_FUNCTIONS_H
