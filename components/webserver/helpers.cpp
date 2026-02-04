#include "helpers.h"
#include <cstdio>
#include <cstring>


namespace minibot::webserver{

const char* HTTP_OK_TEMPLATE =
	"HTTP/1.1 200 OK\r\n"\
	"Content-Type: %s\r\n%s"\
	"Connection: close\r\n\r\n";


// {0}: HTTP Error code
// {1}: HTTP Error code label
// {2}: Message
// {3}: [Post format] content length
const char* HTTP_ERR_TEMPLATE =
	"HTTP/1.1 %d %s\r\n"\
	"Content-Length: %%lu\r\n"\
	"Connection: Close\r\n"\
	"Content-Type: text/html; charset=iso-8859-1\r\n\r\n"\
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"\
	"<html><head><title>%d %s</title></head>\r\n"\
	"<body><h1>%d %s</h1>%s</body></html>\r\n\r\n";



void write_ok_header(char* buffer, uint32_t buff_size, const char* mime_type, uint32_t offset, bool chunked){
	sprintf((char*)(buffer+offset), HTTP_OK_TEMPLATE, mime_type, chunked ? "Transfer-Encoding: chunked\r\n" : "");
}


void write_error_header(char* buffer, uint32_t buff_size,
	uint16_t    error_code,
	const char* error_code_label,
	const char* message,
	uint32_t offset)
{
	sprintf((char*)(buffer+offset), HTTP_ERR_TEMPLATE,
		error_code, error_code_label, // HEADER
		error_code, error_code_label, // Title
		error_code, error_code_label, // Body
		message
	);
}

const char* ext_from_file(const char* file){
	if( !file ) return NULL;
	char* end = (char*)file + strlen(file);

	for(char* cc = (end-1); cc > file; --cc){
		switch(*cc){
			case '.':
				return cc+1;
			case '*': case '?': case '"':  case ',':
			case ';': case ':': case '/':  case '|':
			case '+': case '=': case '<':  case '>':
			case '[': case ']': case '\t': case '\\':
				return end;
		}
	}
	return end;
}


const char* mime_from_ext(const char* ext){
	switch(ext[0]){
		case 't':
			if( !strcmp(ext, "txt") )
				return "text/plain";
			break;
		case 'c':
			if( !strcmp(ext, "css") )
				return "text/css";
			break;
		case 'h':
			if( !strcmp(ext, "htm") || !strcmp(ext, "html") )
				return "text/html";
			break;
		case 'j':
			if( !strcmp(ext, "js") )
				return "text/javascript";
			if( !strcmp(ext, "jpg") || !strcmp(ext, "jpeg") )
				return "image/jpeg";
			break;
		case 'w':
			if( !strcmp(ext, "webp") )
				return "image/webp";
			break;
		case 'p':
			if( !strcmp(ext, "png") )
				return "image/png";
			break;
		case 's':
			if( !strcmp(ext, "svg") )
				return "image/svg+xml";
			break;
	}
	return NULL;
}


}