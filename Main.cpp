#include <iostream>
#include <string>
#include <WinSock2.h>
#include "Headers/ServerFunctions.h"

using namespace std;

int main() {

	int port = 55555;
	SOCKET socks;

	load_WSA_dll();
	socks = socket();
	bindSocket(port,socks);
	return 0;
}