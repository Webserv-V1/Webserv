#ifndef __DEFAULT_CONF_HPP__
#define __DEFAULT_CONF_HPP__

#define WEBSERV_CONF_PATH "./conf/webserv.conf"
#define DEFAULT_HTML_PATH "./html_file"
#define MIME_TYPE_PATH "./include/mime.types.default" 
//설명 : mime.type은 HTTP 헤더로 주는 데이터 파일명 확인하는 파일.

#define MAX_FD				1024
#define BUF_SIZE 			2048
#define	RQ_LINE_NOT_PARSED	0
#define	HEADER_NOT_PARSED	1
#define	NO_BODY				2
#define	TRANSFER_ENCODING	3
#define	CONTENT_LENGTH		4
#define MULTIPART			5

#endif
