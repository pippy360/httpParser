#include "httpParserCommon.h"

#define HTTP_HEADER_PARSER_MAX_CHUNK_SIZE_STR_BUFFER_LENGTH 		40
#define HTTP_HEADER_PARSER_MAX_METHOD_BUFFER_LENGTH					100
#define HTTP_HEADER_PARSER_MAX_PATH_BUFFER_LENGTH 					1000
#define HTTP_HEADER_PARSER_MAX_INNER_HEADER_BUFFER_LENGTH 			1000
#define HTTP_HEADER_PARSER_MAX_FIRST_FEW_BYTES_LENGTH 				10
#define HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH 		10
#define HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH 	10
#define HTTP_HEADER_PARSER_MAX_STATUS_CODE_BUFFER_LENGTH			4
#define HTTP_HEADER_PARSER_MAX_REASON_PHRASE_BUFFER_LENGTH			20
#define MAX_SIZE_OF_INNER_HEADER_NAME								1000
#define MAX_SIZE_OF_INNER_HEADER_VALUE								1000

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
	HEADER_RESPONSE_NEW_LINE_R, //\r
	HEADER_RESPONSE_FINISHED_PARSING, //\r
	ERROR_HEADER_RESPONSE_PARSER,
} StatusLineResponseHeaderParserStateEnum;

typedef enum {
	HEADER_REQUEST_START,
	HEADER_REQUEST_METHOD,
	HEADER_REQUEST_SPACE_AFTER_METHOD,
	HEADER_REQUEST_PATH,
	HEADER_SPACE_AFTER_PATH,
	HEADER_REQUEST_HTTP_VERSION_H,
	HEADER_REQUEST_HTTP_VERSION_T,
	HEADER_REQUEST_HTTP_VERSION_T_2,
	HEADER_REQUEST_HTTP_VERSION_P,
	HEADER_REQUEST_HTTP_VERSION_SLASH,
	HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER,
	HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER,
	HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER,
	HEADER_REQUEST_NEW_LINE_R, //\r
	HEADER_REQUEST_FINISHED_PARSING, //\r
	ERROR_HEADER_REQUEST_PARSER,
} StatusLineRequestHeaderParserStateEnum;

typedef enum {
	INNER_HEADER_START,
	INNER_HEADER_NAME,
	INNER_HEADER_COLON,
	INNER_HEADER_VALUE,
	INNER_HEADER_NEW_LINE_R, //\r
	INNER_HEADER_NEW_LINE_N, //\n
	INNER_HEADER_FINAL_NEW_LINE_R, //\r
	INNER_HEADER_FINISHED_PARSING,
	ERROR_INNER_HEADER_PARSER,
} InnerHeadersParserStateEnum;

typedef enum {
	HEADERBYTETYPE_HTTP_PROTOCOL_BYTE, // "HTTP/"
	HEADERBYTETYPE_HTTP_PROTOCOL_VERSION_BYTE, // "1.1"
	HEADERBYTETYPE_NEW_LINE_R, //\r
	HEADERBYTETYPE_NEW_LINE_N, //\n
	HEADERBYTETYPE_WHITESPACE_SPACE, //' '
	HEADERBYTETYPE_HEADER_PATH_BYTE, // '/index.html'
	HEADERBYTETYPE_INNER_HEADER_NAME_BYTE,
	HEADERBYTETYPE_COLON_BYTE, //':'
	HEADERBYTETYPE_INNER_HEADER_VALUE_BYTE,
} HeaderByteType;

typedef enum {
	HTTP_PACKET_TYPE_REQUEST, HTTP_PACKET_TYPE_RESPONSE,
} HttpRequestOrResponseType;

typedef struct {
	StatusLineRequestHeaderParserStateEnum stateVal; //state enum
	int httpVersionFirstNumber;
	int httpVersionSecondNumber;
	//method
	char methodBuffer[HTTP_HEADER_PARSER_MAX_METHOD_BUFFER_LENGTH];
	int methodBufferLength;
	int maxSizeOfMethodBuffer; //const int maxSizeOfMethodBuffer = HTTP_HEADER_PARSER_MAX_METHOD_BUFFER_LENGTH;
	//path
	char path[HTTP_HEADER_PARSER_MAX_PATH_BUFFER_LENGTH];
	int pathLength;
	int maxSizeOfPathBuffer;// = HTTP_HEADER_PARSER_MAX_PATH_BUFFER_LENGTH;
	//http version buffers
	char httpVersionFirstNumberBuffer[HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH];
	int httpVersionFirstNumberBufferLength;
	int maxSizeOfHttpVersionFirstNumberBuffer;// = HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH;

	char httpVersionSecondNumberBuffer[HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH];
	int httpVersionSecondNumberBufferLength;
	int maxSizeOfHttpVersionSecondNumberBuffer;// = HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH;
	//errors
	int isError;
	HTTPParserErrorState errorState;
} HeaderParserStatusLineParserStateRequest;

const HeaderParserStatusLineParserStateRequest headerParserStatusLineParserStateRequest_init =
		{ HEADER_REQUEST_START, 0, 0, "\0", 0,
		HTTP_HEADER_PARSER_MAX_METHOD_BUFFER_LENGTH, "\0", 0,
		HTTP_HEADER_PARSER_MAX_PATH_BUFFER_LENGTH, "\0", 0,
		HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH, "\0", 0,
		HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH, false, NO_ERROR };

typedef struct {
	StatusLineResponseHeaderParserStateEnum stateVal; //state enum
	int httpVersionFirstNumber;
	int httpVersionSecondNumber;
	//http version buffers
	char httpVersionFirstNumberBuffer[HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH];
	int httpVersionFirstNumberBufferLength;
	int maxSizeOfHttpVersionFirstNumberBuffer; //const int maxSizeOfHttpVersionFirstNumberBuffer = HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH;

	char httpVersionSecondNumberBuffer[HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH];
	int httpVersionSecondNumberBufferLength;
	int maxSizeOfHttpVersionSecondNumberBuffer; //const int maxSizeOfHttpVersionSecondNumberBuffer = HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH;
	//status code
	char httpStatusCodeBuffer[HTTP_HEADER_PARSER_MAX_STATUS_CODE_BUFFER_LENGTH];
	int httpStatusCodeBufferLength;
	int maxSizeOfHttpStatusCodeBuffer;//const int maxSizeOfHttpStatusCodeBuffer = HTTP_HEADER_PARSER_MAX_STATUS_CODE_BUFFER_LENGTH;
	//reason phrase
	char httpReasonPhraseBuffer[HTTP_HEADER_PARSER_MAX_REASON_PHRASE_BUFFER_LENGTH];
	int httpReasonPhraseBufferLength;
	int maxSizeOfHttpReasonPhraseBuffer;//const int maxSizeOfHttpReasonPhraseBuffer = HTTP_HEADER_PARSER_MAX_REASON_PHRASE_BUFFER_LENGTH;
	//errors
	int isError;
	HTTPParserErrorState errorState;
} HeaderParserStatusLineParserStateResponse;

const HeaderParserStatusLineParserStateResponse HeaderParserStatusLineParserStateResponse_init =
		{ HEADER_RESPONSE_START, 0, 0, "\0", 0,
		HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH, "\0", 0,
		HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH, "\0", 0,
		HTTP_HEADER_PARSER_MAX_STATUS_CODE_BUFFER_LENGTH, "\0", 0,
		HTTP_HEADER_PARSER_MAX_REASON_PHRASE_BUFFER_LENGTH, false, NO_ERROR };

typedef enum {
	HEADER_PARSER_STATUS_LINE_PARSER_REQUEST,
	HEADER_PARSER_STATUS_LINE_PARSER_RESPONSE,
	HEADER_PARSER_STATUS
} CurrentActiveStatusLineParser;

typedef struct {
	CurrentActiveStatusLineParser currentActiveStatusLineParser;//depends if the packet is a http request/http response
	HeaderParserStatusLineParserStateResponse responseStatusLineState;
	HeaderParserStatusLineParserStateRequest requestStatusLineState;
} HeaderParserStatusLineParserState;

const HeaderParserStatusLineParserState headerParserStatusLineParserState_request_init = {
		HEADER_PARSER_STATUS_LINE_PARSER_REQUEST,
		HeaderParserStatusLineParserStateResponse_init,
		headerParserStatusLineParserStateRequest_init
};

const HeaderParserStatusLineParserState headerParserStatusLineParserState_response_init = {
		HEADER_PARSER_STATUS_LINE_PARSER_RESPONSE,
		HeaderParserStatusLineParserStateResponse_init,
		headerParserStatusLineParserStateRequest_init
};

typedef enum {
	HEADER_PARSER_STATUS_LINE, HEADER_PARSER_INNER_HEADERS,
} CurrentActiveHeaderParser;

typedef struct {
	InnerHeadersParserStateEnum stateVal;	//state enum
	char innerHeaderName[MAX_SIZE_OF_INNER_HEADER_NAME];
	int innerHeaderNameLength;
	int maxSizeOfInnerHeaderName;//const int maxSizeOfInnerHeaderName = MAX_SIZE_OF_INNER_HEADER_NAME;
	char innerHeaderValue[MAX_SIZE_OF_INNER_HEADER_VALUE];
	int innerHeaderValueLength;
	int maxSizeOfInnerHeaderValue;//const int maxSizeOfInnerHeaderValue = MAX_SIZE_OF_INNER_HEADER_VALUE;
	//errors
	int isError;
	HTTPParserErrorState errorState;
} InnerHeadersParserState;

const InnerHeadersParserState innerHeadersParserState_init = {
		INNER_HEADER_START, "\0", 0, MAX_SIZE_OF_INNER_HEADER_NAME, "\0", 0,
		MAX_SIZE_OF_INNER_HEADER_VALUE, false, NO_ERROR };

typedef struct {
	//the requested byte range
	bool isRangeHeaderSet;
	bool isEndRangeSet;
	int startRange;
	int endRange;
} RequestContentRangeInfo;

typedef struct {
	//the returned byte range
	bool isRangeHeaderSet;
	int startRange;
	int endRange;
} ResponseContentRangeInfo;

typedef struct {
	CurrentActiveHeaderParser currentActiveHeaderParser;
	HeaderParserStatusLineParserState statusLineParserState;
	InnerHeadersParserState innerHeadersParserState;
	//parsed info
	HttpPayloadEncoding payloadEncoding;
	//range info
	RequestContentRangeInfo 	requestContentRange;
	ResponseContentRangeInfo 	responseContentRange;
	//errors
	int isError;
	HTTPParserErrorState errorState;
} HeaderParserState;

const HeaderParserState headerParserState_request_init = {
		HEADER_PARSER_STATUS_LINE,
		headerParserStatusLineParserState_request_init,
		innerHeadersParserState_init,
		false,
		NO_ERROR
};

const HeaderParserState headerParserState_response_init = {
		HEADER_PARSER_STATUS_LINE,
		headerParserStatusLineParserState_response_init,
		innerHeadersParserState_init,
		false,
		NO_ERROR
};

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
} ReturnValueOfGetNextRequestStatusLineStateAndByteType;

typedef struct {
	InnerHeadersParserStateEnum innerHeadersParserStateEnum;
	//RequestStatusLineByteType byteType; byte type is not used for the moment
	int isError;
	HTTPParserErrorState errorState;
} ReturnValueOfGetNextInnerHeaderStateAndByteType;



HeaderParserState httpHeaderParserProcessBuffer(const HeaderParserState state,
		const char *packetBuffer, const int packetBufferLength,
		char **parserBufferEndPtr,
		HttpParserCallbackFunction *callbackFunctions,
		HttpRequestOrResponseType packetType);
