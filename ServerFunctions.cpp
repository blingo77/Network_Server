#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Headers/ServerFunctions.h"
#include <tchar.h>
#include <string>
#include <vector>
#include <mutex>

/*
	Server Function Steps:

	1. Initialize WSA - WSAStartup()
	2. Create a socket - socket()
	3. Bind the socket - bind()
	4. Listen on the socket - listen()
	5. Accept a connection - accept(), connect()
	6. Receive and send data - recv(), send(), recvfrom(), sendto()
	7. Disconnect - closesocket()
*/

using namespace std;

mutex mtx;
vector<SOCKET> allClientSockets;

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

		cout << "bindSocket() failed: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		cout << "bindSocket() is OK! " << endl;
	}

	return 0;
}

// makes a socket in a state in which it is listening for a incoming connection
int listen(SOCKET serverSocket) {

	/*
		listen function:

		int listen(SOCKET serverSocket, int backlog)

		serverSocket: an unconnected Socket
		backlog: the maximum number of connections allowed
	*/

	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		cout << "listen(): Error listening on socket: " << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "listen () is OK, Waiting for connections..." << endl;
	}

	return 0;
}

// pauses the server until the user connects with the server
SOCKET acceptSocket(SOCKET serverSocket) {

	/*
		accept() function:

		- this is a blocking function

		SOCKET accept(SOCKET s, struct sockaddr* addr, int* addrlen)
		
		-s: descriptor that identifies a socket that has been placed in a listening
		state with the listen() function

		-addr: optional structure containing the client address information
		-addrlen: optional size of the address structure

		-if no errors, accept() returns a value of type SOCKET that is a descriptor
		for the new socket that is connected to the client. The original socket can be 
		used to listen for more incoming calls.
	*/

	SOCKET acceptedSocket;	// the new socket that accept() will return
	acceptedSocket = accept(serverSocket, NULL, NULL);

	if (acceptedSocket == INVALID_SOCKET) {
		cout << "accept failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}
	else {
		cout << "Connected.." << endl;

		lock_guard<mutex> lock(mtx);
		allClientSockets.push_back(acceptedSocket);
	}

	return acceptedSocket;
}

void boradCastMessage(string message, SOCKET senderSocket) {

	lock_guard<mutex> lock(mtx);

	for (SOCKET client : allClientSockets) {

		send(client, message.c_str(), message.size(), 0);

		
	}
}

int receiveData(SOCKET acceptedSocket) {

	/*
		recv(function):

		- this function recieves data from a connected socket.

		int recv(SOCKET s, char* buf, int len, int flags)

		-s: The descriptor that identifies a connected socket.
		-buf: A pointer to the buffer to recieve incoming data.
		-len: The length in bytes of the buffer pointed to by the buf parameter.
		-flags: optional set of flags that influences the behavior of this function

		- if no errors, then recv returns the number of bytes received. If the
		connection has been closed gracefully, the return value is zero. Otherwise
		SOCKET_ERROR is returned
	*/

	char receiveBuffer[200] = "";
	
	while (true) {

		int byteCount = recv(acceptedSocket, receiveBuffer, 200, 0);

		string message = "Client " + to_string(acceptedSocket) + ":" + receiveBuffer + "\n";
		cout << message << endl;

		if (byteCount < 0) {
			cout << "Client: error" << WSAGetLastError() << endl;
			return 0;
		}
		else {
			cout << "Received data: " << receiveBuffer << endl;
		}

		boradCastMessage(message, acceptedSocket);
	}
}