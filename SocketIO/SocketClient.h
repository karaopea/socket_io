#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#include <string>
#define BUFSIZE (1280*720+1000)

class SocketClient
{
public:
	int connect(char* ip, int port);
	int send(const char* data);
	std::string receive();
	int disconnect();
private:
	int retval;
	char buf[BUFSIZE+1];
	WSADATA wsa;
	SOCKET sock;
};