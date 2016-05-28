#ifndef __HTTPCOMMON_H_INCLUDED__
#define __HTTPCOMMON_H_INCLUDED__

#define HTTP_COMMON_MAX_PATH_BUFFER_LENGTH 1000

typedef enum {
	HTTP_PACKET_RESPONSE,
	HTTP_PACKET_REQUEST,
} HttpPacketType;

typedef enum {
	REQUEST_GET,
	REQUEST_HEAD,
	REQUEST_POST,
	REQUEST_PUT,
	REQUEST_DELETE,
	REQUEST_TRACE,
	REQUEST_OPTIONS,
	REQUEST_CONNECT,
	REQUEST_PATCH,
	ERROR_REQUEST_BAD_REQUEST_METHOD
} HttpRequestMethod;

typedef enum {
	HTTP_PAYLOAD_CHUNKED,
	HTTP_PAYLOAD_CONTENT_LENGTH,
} HttpPayloadEncoding;

typedef struct {
	HttpPacketType 		requestOrResponse;
	HttpPayloadEncoding payloadEncoding;
	HttpRequestMethod 	requestMethod;
	int 	statusCode;
	int 	httpVersionFirstNumber;
	int 	httpVersionSecondNumber;
	char 	path[HTTP_COMMON_MAX_PATH_BUFFER_LENGTH];
	int 	pathLength;
	const int pathBufferMaxLength = HTTP_COMMON_MAX_PATH_BUFFER_LENGTH;
} HttpPacketInfo;

#endif //__HTTPCOMMON_H_INCLUDED__

