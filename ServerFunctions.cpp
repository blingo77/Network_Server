#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Headers/ServerFunctions.h"
#include <tchar.h>

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

// Load the Windows Socket API dll file
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

// Creates a socket 
SOCKET socket(void) {

	SOCKET serverSocket = socket(AF_INET,		// af (address family): 'AF_INET' means for UDP or TCP
								SOCK_STREAM,	// type: SOCK_STREAM for TCP and SOCK_DGRAM for UDP
								IPPROTO_TCP);	// protocol: IPPROTO_TCP for TCP

	if (serverSocket == INVALID_SOCKET) {

		cerr << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(serverSocket);	// close the socket
		return 0;
	}
	else {
		cout << "socket() is OK!" << endl;
	}

	return serverSocket;
}

// Associates a local address with a socket
int bindSocket(int port, SOCKET serverSocket) {

	/*
		sockaddr_in sturcture:

		Used by Window Sockets to specify a local or remote endpoint address to which
		to connect a socket

		struct sockaddr_in{
			short sin_family;			// Adress family (Must be AF_INET)
			unsigned short sin_port;	// the IP port
			struct in_addr sin_addrl	// the IP address
			char sin_zero[8]			// padding to make the structure the same size
	*/

	sockaddr_in service;			// specifies an endpoint address for the socket
	service.sin_family = AF_INET;	// sets the address family for the socket

	// converts the IP address "127.0.0.1" into its binary form and stores it in service.sin_addr.s_addr
	InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
	service.sin_port = htons(port);	// returns the value of the port in TCP/IP network byte order

	// attempts to bind 'serverSocket' to the specified address and port in the 'service' structure
	if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {

		cout << "bindSocket() failed: " << WSAGetLastError << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		cout << "bindSocket() is OK! " << endl;
	}

	return 0;
}