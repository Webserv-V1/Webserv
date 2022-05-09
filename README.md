# webserv
# 1. 프로젝트 소개 😮
### 소개 :
* 크롬 클라이언트와 TCP/IP 통신하며, 멀티플렉싱을 통해 하나의 채널로 여러 클라이언트 요청을 처리하는 서버입니다. 
* 클라이언트의 HTTP 요청을, 서버가 받고 HTTP 응답 해주는 서버입니다.
 
### 기간 :
* 2022.03.08 ~ 2022.05.04

### 사용법 : 
```
make
./webserv 또는 ./webserv "작성한 conf파일"
```

### 프로젝트 구현 명세서
* 아래 내용들이 구현되어있습니다.

https://github.com/Webserv-V1/Webserv/wiki/%ED%94%84%EB%A1%9C%EC%A0%9D%ED%8A%B8-%EB%AA%85%EC%84%B8%EC%84%9C

### 서버 conf파일 작성예시 :
* conf 파일을 통해 서버를 설정합니다. 위 사이트에 conf 파일 예시를 작성해 두었습니다🥸
* nginx 와 apach conf에서 아이디어를 받았지만, 사용법은 조금 다릅니다.

https://github.com/Webserv-V1/Webserv/wiki/config-file-%EC%9E%91%EC%84%B1-%EB%B0%8F-%EC%9D%B4%ED%95%B4.



### 동작모습 :
![화면-기록-2022-05-09-오후-4 08 47 gif crdownload](https://user-images.githubusercontent.com/52343427/167358445-84e6b8e8-5059-4057-b12c-405083b4071f.gif)

# 4. nginx 기본구조.
```
.
├── fastcgi.conf
├── fastcgi.conf.default
├── fastcgi_params
├── fastcgi_params.default
├── koi-utf
├── koi-win
├── mime.types
├── mime.types.default
├── nginx.conf
├── nginx.conf.default
├── scgi_params
├── scgi_params.default
├── servers
├── uwsgi_params
├── uwsgi_params.default
└── win-utf

1 directory, 15 files
```

# 4. 프로젝트 기반 지식 ✍️

