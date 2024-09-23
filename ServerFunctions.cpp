#include <iostream>
#include <WinSock2.h>
#include "Headers/ServerFunctions.h"
using namespace std;

/*
	WSADATA struct:

	typedef struct WSAData{
	    WORD wVersion;         // Winsock version
        WORD wHighVersion;     // Highest version supported
        char szDescription[WSADESCRIPTION_LEN + 1];  // Description of the Winsock implementation
        char szSystemStatus[WSASYSSTATUS_LEN + 1];   // Status or configuration string
        unsigned short iMaxSockets;  // Maximum number of sockets allowed
        unsigned short iMaxUdpDg;    // Maximum size of a UDP datagram
        char* lpVendorInfo;          // Vendor-specific information
	}

*/

int load_WSA_dll(void) {

	WSADATA wsaData;	// WSASdata struct
	int wsaErr;			// WSA Error, if the startup dosent return 0
	
												// Combines two 8-bit values into as single 16 bit value
	WORD wVersionRequested = MAKEWORD(2, 2);	// (2,2) represents Winsock version 2.2
	wsaErr = WSAStartup(wVersionRequested, &wsaData);

	// if WSAStartup is successfull then it will return 0
	if (wsaErr != 0) {
		cout << "Winsock dll not found!" << endl;
		return 0;
	}
	else {
		cout << "The Winsock dll found!" << endl;
		cout << "The status: " << wsaData.szSystemStatus << endl;
	}

	return 0;
}

int socket(void) {

	SOCKET serverSocket = socket(AF_INET,		// af (address family): 'AF_INET' means for UDP or TCP
								SOCK_STREAM,	// type: SOCK_STREAM for TCP and SOCK_DGRAM for UDP
								IPPROTO_TCP);	// protocol: IPPROTO_TCP for TCP

	if (serverSocket == INVALID_SOCKET) {

		cerr << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else {
		cout << "socket() is OK!" << endl;
	}

	WSACleanup();	// terminates the Winsock 2 DLL (Ws2_32.dll) and lets other prorams use is it
	closesocket(serverSocket);	// close the socket
	return 0;
}
