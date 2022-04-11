#include "SocketClient.h"

int recvn(SOCKET s, char *buf, int len, int flags);

int SocketClient::connect(char* ip, int port)
{
	// 윈속 초기화
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// socket() 소켓생성 (tcp / udp 결정) 
	sock = socket(AF_INET, SOCK_STREAM, 0); // 소켓만들기 (주소체계,전송방식(tcp),0)
	if (sock == INVALID_SOCKET)
	{
		return -1;
	}

	// connect() 연결 요청을 listening 소캣을 보냄
	SOCKADDR_IN serveraddr;// 소켓 주소 구조체(IPv4 전용)
	ZeroMemory(&serveraddr, sizeof(serveraddr)); // 메모리 비우기
	serveraddr.sin_family = AF_INET; // 주소체계
	serveraddr.sin_addr.s_addr = inet_addr(ip);// 4바이트 아무 호스트주소를 가져와  4바이트 네트워크 정렬 방식으로 변환한뒤 IPv4 주소에 할당
	serveraddr.sin_port = htons(port);// 2바이트 포트번호를 2바이트 네트워크 정렬 방식으로 변환한뒤 포트번호에 할당
	retval = ::connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)); // 생성한 소켓을 통해 서버에 접속 (접속할 소켓, 서버 주소(포인터), 주소크기)
	if (retval == SOCKET_ERROR)
		return -1;

	return 0;
}


std::string SocketClient::receive()
{
	// 데이터 통신에 사용할 변수
	//strcpy_s(buf, "");
	//printf("%s\n", buf);
	
	
	// 데이터 받기

	retval = recv(sock, buf, BUFSIZE, 0);
	if (retval != SOCKET_ERROR) // 수신이 실패 했을 때 err_display 메소드 실행
	{
		buf[retval] = '\0';
	}
	return buf;
}

int SocketClient::send(const char* data)
{
	// 데이터 통신에 사용할 변수
	
	int len;

	strcpy_s(buf, data);

	// '\n' 문자 제거
	len = strlen(buf);
	if (buf[len - 1] == '\n')
		buf[len - 1] = '\0';
	if (strlen(buf) == 0)
		return -1;

	// 데이터 보내기
	retval = ::send(sock, buf, strlen(buf), 0); // 송신할 바이트 수를 리턴 (클라이언트 소켓, 전송할 데이터, 전송할 데이터 길이,0)
	if (retval == SOCKET_ERROR){
		//err_display("send()");
		return -1;
	}


	
	
	//printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
	//printf("[받은 데이터] %s\n", buf);
	return 0;
}

int SocketClient::disconnect()
{
	closesocket(sock);

	// 윈속 종료
	WSACleanup();


	return 0;
}


int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf; // 수신된 데이터의 주소
	int left = len; // 길이

	while (left > 0)
	{
		received = recv(s, ptr, left, flags); // 받은 바이트의 수 리턴
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0) // 바이트 수가 0일 경우
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}
