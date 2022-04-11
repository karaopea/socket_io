#include "SocketServer.h"
#include <stdio.h>

int SocketServer::init(int port)
{
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
		return -1;

	// socket() 
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	// bind() 
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
	retval = ::bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

	printf("[SOCKET SERVER] - %s:%d\n", getIpAddress(), port);

	
	return 0;
}

char* SocketServer::getIpAddress()
{
	char		ac[80];
	char*	strLocalIP = "";
	DWORD		dwErr;

	memset(ac, 0x00, 80);
	if (gethostname(ac, 80) == SOCKET_ERROR) {
		dwErr = GetLastError();
		_snprintf_s(ac, 80, "%d", dwErr);

		return strLocalIP;
	}

	struct hostent *phe = gethostbyname(ac);
	if (!phe) {
		return strLocalIP;
	}
	else {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
		strLocalIP = inet_ntoa(addr);
	}

	return strLocalIP;
}

void SocketServer::pushMessage(std::string str)
{
	squeue.enqueue(str.c_str());
}
	


std::string SocketServer::popMessage()
{
	std::string val;
	if (squeue.getQueueSize() > 0)
		val = squeue.dequeue();

	return val;
}

int SocketServer::send_msg(std::string msg)
{
	strcpy_s(buf, msg.c_str());
	retval = send(client_sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR)
		return -1;
	else
	{
		//printf("SERVER SEND:%s\n", buf);
		buf[retval] = '\0';
		return 0;
	}
		
}

int SocketServer::start()
{
	// listen() 
	retval = listen(listen_sock, SOMAXCONN);

	// ������ ��ſ� ����� ���� 


	while (1) {
		// accept() 
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);

		// ������ Ŭ���̾�Ʈ ���� ��� 

		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 	inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		// Ŭ���̾�Ʈ�� ������ ��� 
		while (1) {
			// ������ �ޱ�
			/*
			retval = recv(client_sock, &x, sizeof(char), 0);
			if (retval == SOCKET_ERROR || retval == 0) break;

			// ���� ������ ���
			printf("%c", x);
			*/
			//retval = recv(client_sock, buf, BUFSIZE, 0);

			std::string msg = popMessage();
			if (!msg.empty())
			{
				if (send_msg(msg) == -1)
					break;
			}
			

		}
		// closesocket() 
		closesocket(client_sock);
	}
	// ���� ���� ���� 
	closesocket(listen_sock);

	// ���� ���� 
	WSACleanup();
	return 0;
}
