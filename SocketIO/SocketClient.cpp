#include "SocketClient.h"

int recvn(SOCKET s, char *buf, int len, int flags);

int SocketClient::connect(char* ip, int port)
{
	// ���� �ʱ�ȭ
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// socket() ���ϻ��� (tcp / udp ����) 
	sock = socket(AF_INET, SOCK_STREAM, 0); // ���ϸ���� (�ּ�ü��,���۹��(tcp),0)
	if (sock == INVALID_SOCKET)
	{
		return -1;
	}

	// connect() ���� ��û�� listening ��Ĺ�� ����
	SOCKADDR_IN serveraddr;// ���� �ּ� ����ü(IPv4 ����)
	ZeroMemory(&serveraddr, sizeof(serveraddr)); // �޸� ����
	serveraddr.sin_family = AF_INET; // �ּ�ü��
	serveraddr.sin_addr.s_addr = inet_addr(ip);// 4����Ʈ �ƹ� ȣ��Ʈ�ּҸ� ������  4����Ʈ ��Ʈ��ũ ���� ������� ��ȯ�ѵ� IPv4 �ּҿ� �Ҵ�
	serveraddr.sin_port = htons(port);// 2����Ʈ ��Ʈ��ȣ�� 2����Ʈ ��Ʈ��ũ ���� ������� ��ȯ�ѵ� ��Ʈ��ȣ�� �Ҵ�
	retval = ::connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)); // ������ ������ ���� ������ ���� (������ ����, ���� �ּ�(������), �ּ�ũ��)
	if (retval == SOCKET_ERROR)
		return -1;

	return 0;
}


std::string SocketClient::receive()
{
	// ������ ��ſ� ����� ����
	//strcpy_s(buf, "");
	//printf("%s\n", buf);
	
	
	// ������ �ޱ�

	retval = recv(sock, buf, BUFSIZE, 0);
	if (retval != SOCKET_ERROR) // ������ ���� ���� �� err_display �޼ҵ� ����
	{
		buf[retval] = '\0';
	}
	return buf;
}

int SocketClient::send(const char* data)
{
	// ������ ��ſ� ����� ����
	
	int len;

	strcpy_s(buf, data);

	// '\n' ���� ����
	len = strlen(buf);
	if (buf[len - 1] == '\n')
		buf[len - 1] = '\0';
	if (strlen(buf) == 0)
		return -1;

	// ������ ������
	retval = ::send(sock, buf, strlen(buf), 0); // �۽��� ����Ʈ ���� ���� (Ŭ���̾�Ʈ ����, ������ ������, ������ ������ ����,0)
	if (retval == SOCKET_ERROR){
		//err_display("send()");
		return -1;
	}


	
	
	//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
	//printf("[���� ������] %s\n", buf);
	return 0;
}

int SocketClient::disconnect()
{
	closesocket(sock);

	// ���� ����
	WSACleanup();


	return 0;
}


int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf; // ���ŵ� �������� �ּ�
	int left = len; // ����

	while (left > 0)
	{
		received = recv(s, ptr, left, flags); // ���� ����Ʈ�� �� ����
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0) // ����Ʈ ���� 0�� ���
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}
