# webserv
# 1. 프로젝트 소개 😮
### 1-1. 소개 :
* 크롬 클라이언트와 TCP/IP 통신하며, 멀티플렉싱을 통해 하나의 채널로 여러 클라이언트 요청을 처리하는 서버입니다. 
* 클라이언트의 HTTP 요청을, 서버가 받고 HTTP 응답 해주는 서버입니다.
 
### 1-2. 개발기간 :
* 2022.03.08 ~ 2022.05.04

### 1-3. 설정 :
* 서버 default 설정파일은 ./conf/default.conf 입니다.
* ./default_conf.hpp 파일을 이용해 설정 파일 및 메크로를 바꿀 수 있습니다. 

### 1-4. 프로젝트 구현 명세서
* [여기](https://github.com/Webserv-V1/Webserv/wiki/%ED%94%84%EB%A1%9C%EC%A0%9D%ED%8A%B8-%EB%AA%85%EC%84%B8%EC%84%9C) 프로젝트에서 구현한 내용들을 확인할 수 있습니다. 


### 1-5. 서버 conf파일 작성예시 :
* conf 파일을 통해 서버를 설정합니다.  [여기](https://github.com/Webserv-V1/Webserv/wiki/config-file-%EC%9E%91%EC%84%B1-%EB%B0%8F-%EC%9D%B4%ED%95%B4.) conf 파일 예시를 확인할 수 있습니다.🥸
* nginx 와 apach conf에서 아이디어를 받았지만, 사용법은 조금 다릅니다.

### 1-6. 사용법 : 
* 아래와 같은 방법으로 서버를 실행시킬 수 있습니다. 

```
make
./webserv 또는 ./webserv "작성한 conf파일"
```

# 2. nginx 기본구조.
* 파일 구조는 아래와 같습니다. 

```
.
├── Makefile
├── README.md 
├── cgi-bin				: php실행 파일과 *.php 파일이 있습니다.
├── conf				: 서버 설정파일이 있습니다.
├── default_conf.hpp	: 메크로를 설정합니다.
├── html_file			: html 파일이 있습니다.
├── http_msg_txt		: http_msg 예시입니다.
├── include				: 헤더파일이 있습니다.
└── srcs				: 소스파일을 모아두었습니다.
```

# 3. 프로젝트 기반 지식 ✍️
* webserv wiki에 정리한 필요한 이론들을 [여기](https://github.com/Webserv-V1/Webserv/wiki) 정리해두었습니다!
<br/><br/>

# 4. TEST
### 4-1. 크롬
기본 GET 요청

```
[GET] localhost:8080/  
[GET] localhost:8080/index.html  
[GET] localhost:8080/asuhf 
[GET] localhost:8080/1 ~ [GET] localhost:8080/11
```
![화면-기록-2022-05-09-오후-5 38 12](https://user-images.githubusercontent.com/52343427/167373455-a4f6ddfb-7dac-4707-b6d9-d217ce78964c.gif)
<br/><br/>

GET qurry string, POST 요청
```
[GET qurrry stirng] http://localhost:8080/cgi-bin/get_name.php?first_name=hoyong&last_name=LEE
[POST] http://localhost:8080/cgi-bin/get_profile.php
[POST] http://localhost:8080/cgi-bin/upload.php
```

![화면-기록-2022-05-09-오후-6 01 06](https://user-images.githubusercontent.com/52343427/167378517-e61f4080-7995-4bb7-baa0-5be0c4975da2.gif)
<br/><br/>

### 4-2. telnet
telnet을 통한 GET POST DELETE 요청

```
GET / HTTP/1.1
Host: localhost:8080

POST /cgi-bin/get_name.php HTTP/1.1
Host: localhost:8080

DELETE /9/test.html HTTP/1.1
Host: localhost:8080

```

![화면-기록-2022-05-09-오후-6 08 51](https://user-images.githubusercontent.com/52343427/167386482-ab5880c5-7cf8-4a04-9e3f-e080324501d9.gif)
<br/><br/>

### 4-3. siged
siege 를 통한 스트레스 테스트

```
brew install siege
siege -b 127.0.0.1:8080
```

![화면-기록-2022-05-09-오후-7 09 25](https://user-images.githubusercontent.com/52343427/167389381-30d6fa87-6b21-415d-834f-a8395e756bc2.gif)
