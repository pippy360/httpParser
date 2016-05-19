#include "httpParserCommon.h"

#define MAX_CHUNK_SIZE_STR_BUFFER_LENGTH 40
#define MAX_PATH_BUFFER_LENGTH 1000
#define MAX_INNER_HEADER_BUFFER_LENGTH 1000
#define MAX_FIRST_FEW_BYTES_LENGTH 10

typedef enum {
	HEADER_RESPONSE_START,
	HEADER_RESPONSE_HTTP_VERSION_H,
	HEADER_RESPONSE_HTTP_VERSION_T,
	HEADER_RESPONSE_HTTP_VERSION_T_2,
	HEADER_RESPONSE_HTTP_VERSION_P,
	HEADER_RESPONSE_HTTP_VERSION_SLASH,
	HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER,
	HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER,
	HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER,
	HEADER_RESPONSE_SPACE_AFTER_VERSION,
	HEADER_RESPONSE_STATUS_CODE,
	HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE,
	HEADER_RESPONSE_REASON_PHRASE,
	HEADER_RESPONSE_NEW_LINE_R,//\r
	HEADER_RESPONSE_FINISHED_PARSING,//\r
	ERROR_HEADER_RESPONSE_PARSER,
} StatusLineRequestHeaderParserStateEnum;


typedef enum {
	HEADER_REQUEST_START,
	HEADER_REQUEST_METHOD,
	HEADER_REQUEST_SPACE_AFTER_METHOD,
	HEADER_REQUEST_PATH,
	HEADER_REQUEST_HTTP_VERSION_H,
	HEADER_REQUEST_HTTP_VERSION_T,
	HEADER_REQUEST_HTTP_VERSION_T_2,
	HEADER_REQUEST_HTTP_VERSION_P,
	HEADER_REQUEST_HTTP_VERSION_SLASH,
	HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER,
	HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER,
	HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER,
	HEADER_REQUEST_NEW_LINE_R,//\r
	HEADER_REQUEST_FINISHED_PARSING,//\r
	ERROR_HEADER_REQUEST_PARSER,
} StatusLineResponseHeaderParserStateEnum;


typedef enum {
	INNER_HEADER_START,
	INNER_HEADER_NAME,
	INNER_HEADER_COLON,
	INNER_HEADER_VALUE,
	HEADER_RESPONSE_NEW_LINE_R,//\r
	HEADER_RESPONSE_NEW_LINE_N,//\n
	HEADER_RESPONSE_FINAL_NEW_LINE_R,//\r
	INNER_HEADER_FINISHED_PARSING,
	ERROR_INNER_HEADER_PARSER,
} InnerHeadersParserStateEnum;


typedef enum {
	HEADER_REQUEST_STATUS_LINE,
	HEADER_RESPONSE_STATUS_LINE,
	HEADER_INNER_HEADERS
} HeaderParserStateEnum;

typedef enum{
	HTTP_PROTOCOL_BYTE, // "HTTP/"
	HTTP_PROTOCOL_VERSION_BYTE, // "1.1"
	NEW_LINE_R,//\r
	NEW_LINE_N,//\n
	WHITESPACE_SPACE, //' '
	HEADER_PATH_BYTE, // '/index.html'
	INNER_HEADER_NAME_BYTE,
	COLON_BYTE, //':'
	INNER_HEADER_VALUE_BYTE,
} HeaderByteType;

typedef enum{
	HTTP_PACKET_TYPE_REQUEST,
	HTTP_PACKET_TYPE_RESPONSE,
} HttpRequestOrResponseType;

typedef struct {
	HeaderParserStateEnum stateVal;//check to see if we're in
	long totalBytesProcessedState;
	int httpVersion;
	StatusLineResponseHeaderParserStateEnum statusLineResponseParserState;
	StatusLineRequestHeaderParserStateEnum statusLineRequestParserState;
	InnerHeadersParserStateEnum innerHeadersParserState;
	char firstFewBytesOfHttpPacketBuffer[MAX_FIRST_FEW_BYTES_LENGTH];//bytes for the request (GET, HEAD, PATCH) or the (HTTP) in (HTTP/1.1)
	int firstFewBytesOfHttpPacketBufferLength;
	HttpRequestMethod requestMethod;
	char pathBuffer[MAX_PATH_BUFFER_LENGTH];
	char innerHeaderBuffer[MAX_INNER_HEADER_BUFFER_LENGTH];
	char *innerHeaderNameStart;
	int innerHeaderNameLength;
	char *innerHeaderValueStart;
	int innerHeaderValueLength;
	int isError;
	const char *errorString;
} HeaderParserState;

typedef struct {
	StatusLineResponseHeaderParserStateEnum statusLineParserStateEnum;
	//ResponseStatusLineByteType byteType;
	int isError;
	HTTPParserErrorState errorState;
	const char *errorString;
} ReturnValueOfGetNextResponseStatusLineStateAndByteType;

typedef struct {
	StatusLineRequestHeaderParserStateEnum statusLineParserStateEnum;
	//RequestStatusLineByteType byteType; byte type is not used for the moment
	int isError;
	HTTPParserErrorState errorState;
	const char *errorString;
} ReturnValueOfGetNextRequestStatusLineStateAndByteType;

typedef struct {
	InnerHeadersParserStateEnum innerHeadersParserState;
	//RequestStatusLineByteType byteType; byte type is not used for the moment
	int isError;
	HTTPParserErrorState errorState;
	const char *errorString;
} ReturnValueOfGetNextInnerHeaderStateAndByteType;
