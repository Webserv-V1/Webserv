# webserv

# 1. 프로젝트 소개 😮
### 소개 :
* nginx 서버 만들었습니다.

### 기간 :
* 2022.03.08 ~ 

### 요약 :
1. 이 과제는 HTTP 서버인 nginx를 만듭니다.
2. 실제 HTTP RFC와 다릅니다. 
3. 실제 브라우저로 테스트 했습니다.
4. 클라이언트와 서버 간의 통신은 **Hypertext Transfer** 프로토콜을 사용한다고 가정합니다.
5. 트래픽이 많은 웹 사이트에 여러 웹 서버를 사용할 수 있도록 구현햇습니다.

6. 일반적으로 웹 브라우저 또는 웹 크롤러와 같은 사용자 에이전트는 다음을 통해 통신을 시작합니다.
	* HTTP를 사용하여 특정 리소스를 요청하면 서버가 해당 리소스의 내용으로 응답하거나, 그렇게 할 수 없는 경우에는 오류 메시지로 응답합니다.
	* 리소스는 일반적으로 서버의 보조 스토리지에 있는 실제 파일이지만, 반드시 그런 것은 아니며 웹 서버가 구현되는 방법에 따라 다르게 구현해도 됩니다.

7. 주요 기능은 콘텐츠를 제공하는 것이지만, HTTP의 전체 구현은 클라이언트로부터 콘텐츠를 받는 방법도 포함됩니다. 이 기능은 파일 업로딩을 포함한 웹폼 제출에 사용됩니다. 

# 2. 세부 명세서(Mandatory part) 🐹
### 허용함수
```
Everything in C++ 98. htons, htonl, ntohs, ntohl,
select, poll, epoll (epoll_create, epoll_ctl,
epoll_wait), kqueue (kqueue, kevent), socket,
accept, listen, send, recv, bind, connect,
inet_addr, setsockopt, getsockname, fcntl.
```

1. FD_SET, FD_CLR, FD_ISSET, FD_ZERO 같은 모든 메크로와 정의를 사용할 수 있습니다. (어떻게 쓸지 이해하는건 매우 중요)
2. HTTP 서버는 C++ 98로 작성해야 합니다.
3. 더 많은 C 함수가 필요한 경우 사용할 수 있지만 항상 C++를 선호합니다.
4. 컴파일은 C++ 98이어야 합니다.
5. 외부라이브러리 없음, 부스트 없음, 등.
6. 가능한 c++을 써라(예를 들어 헤더파일에  cstring로 표기해라. string.h가 아니라)
7. 서버는 선택한 웹 브라우저와 호환되어야 합니다.
8. nginx는 HTTP 1.1을 준수하며 헤더 및 응답 동작을 비교하는 데 사용할 수 있습니다.
9. 주제와 규모에서 여론을 통해 언급 할 것입니다. 그러나 select, kqueue, epoll 는 동등하게 동작합니다.
10. 논블로킹이어야하며, 클라이언트와 서버 사이의 모든 IO에 대해 select 1개만 사용해야합니다(listen 포함).
11. poll(또는 이와 동등한 것)은 읽기와 쓰기를 동시에 확인해야 합니다.
12. 서버를 절대 블로킹이 되면 안되고, 필요한 경우 클라이언트가 제대로 바운스되어야 합니다
13. poll(또는 이와 동등한 것)을 거치지 않고 읽기 작업이나 쓰기 작업을 수행해서는 안 됩니다.
14. 읽기 또는 쓰기 작업 후 errno 값 확인은 엄격히 금지됩니다.
15. 서버에 대한 요청이 영원히 중단되어서는 안 됩니다.
16. none이 제공되면, 당신의 서버에는 기본 오류 페이지가 있어야합니다.
17. 프로그램이 누출되어서는 안 되며 충돌도 발생하지 않아야 합니다(모든 초기화가 완료된 경우 메모리가 부족하더라도).
18. 포크를 CGI 이외의 다른 용도로 사용할 수 없습니다(예: php 또는 python 등...).
19. 당신은 다른 웹 서버를 실행할 수 없습니다 ... (You can’t execve another webserver...무슨말..) 
20. 당신의 프로그램은 argument경로나, default경로에 config file을 가진다.
21. config file을 읽기 전에 poll(또는 이와 동등한 것)을 사용할 필요가 없습니다.
22. 완전히 정적인 웹사이트를 제공할 수 있어야 합니다. (You should be able to serve a fully static website.)
23. 클라이언트가 파일을 업로드 할 수 있어야한다
24. HTTP 응답 status codes 는 정확해야 합니다.
25. 최소한 GET, POST 및 DELETE 메소드 필요합니다.
26. stress test는 서버가 어떤 비용을 치르더라도 계속 사용할 수 있어야 합니다
27. 당신의 서버는 여러 포트에서 수신대기 할 수 있다.(See config file).

>정보) mac os X는 write를 다른 Unix OS와 같은 방식으로  구현하지 않기 때문에 fcntl을 사용할 수 있습니다. 다른 OS와 비슷한 결과를 얻으려면 non-blocking FD를 사용해야 합니다.

>주의) 당신이 non-blocking FD를 사용하고 있기 때문에 read/recv 또는 write/send 함수를 select 없이 사용하면 서버가 블로킹되지 않을 것입니다. 다시 한 번 알려드립니다. select를 통하지 않고 어느 FD에서든 read/recv 또는 write/send를 시도하는 것은 평가에서 0점 처리 됩니다.

>주의) 당신은 다음과 같이 fcntl을 사용할 수 있습니다. : fcntl(fd, F_SETFL, O_NONBLOCK);
	다른 모든 flag는 금지합니다.

# 3. config file에서 다음을 수행 할 수 있어야합니다. 😾
>당신은 ft_services나 진행했던 서버 파트에서 configuration file에 대한 영감을 받을 수 있습니다. 

* 각 서버의 포트나 호스트를 선택하세요.



# 4. 프로젝트 기반 지식 ✍️

