#pragma once

#include <windows.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
//#include "winsock2.h"
#include "SafeQueue.h"


#define BUFSIZE (1280*720+1000)

class SocketServer
{
public:
	int init(int port);
	int start();
	int send_msg(std::string msg);
	std::string popMessage();
	void pushMessage(std::string str);
	int getQueueSize(){ return squeue.getQueueSize(); }

private:
	char* getIpAddress();

	int retval;
	// 윈속 초기화 
	WSADATA wsa;
	SOCKET listen_sock;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	SafeQueue<std::string> squeue;
	char buf[BUFSIZE + 1];
	//char x;
};