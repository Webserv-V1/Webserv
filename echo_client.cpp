#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
void error_handling(const char *message) 
{
	std::cerr << message << std::endl;
	exit(1);
}

int main(int argc, char *argv[]) {
    // 파일 디스크립터를 위한 변수
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	if (argc != 3) {
		std::cout << "Usage : " << argv[0] << "<IP> <port>" << std::endl;
		exit(1);
	}

    // 1. socket 하나를 생성한다.
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

    // 2. socket을 이용해 server의 server socket(listen socket)에 연결을 요청한다.
	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
		std::cout << "Connected..........." << std::endl;

	while(1) {
		std::cout << "Input message(Q to quit): ";
		std::cin >> message;

		if (!strcmp(message,"q") || !strcmp(message,"Q"))
			break;

        // 3. 연결된 socket을 통해 server로부터 데이터를 송수신한다.
		write(sock, message, strlen(message));
		str_len = read(sock, message, BUF_SIZE-1);
		message[str_len] = 0;
		std::cout << "Message from server: " << message << std::endl;
	}

	close(sock);
	return 0;
}
