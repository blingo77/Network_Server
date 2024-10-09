#include <iostream>
#include <string>
#include <WinSock2.h>
#include "Headers/ServerFunctions.h"
#include <thread>
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

int main() {

	int port = 55555;
	SOCKET socks;
	SOCKET acceptedSocket;
	vector<SOCKET> allClientSockets;
	mutex clientsMutex;


	load_WSA_dll();
	socks = socket();
	bindSocket(port,socks);
	listen(socks);

	// multi threading to allow for multiple clients
	while (true){
		acceptedSocket = acceptSocket(socks);

		if (acceptedSocket == INVALID_SOCKET) {
			// break from while loop if the socket is invalid
			break;
		}

		thread clientThread(receiveData, acceptedSocket);	// pass in recieve function and its SOCKET paremeter
		clientThread.detach();		// allows the thread to run independent of the main thread
	}

	closesocket(socks);
	WSACleanup();
	return 0;
}