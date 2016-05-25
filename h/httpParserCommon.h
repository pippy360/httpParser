#ifndef __HTTPPARSERCOMMON_H_INCLUDED__
#define __HTTPPARSERCOMMON_H_INCLUDED__
#define PARSER_ERROR(VALUE) HTTPParserErrorStateErrorStrings[VALUE]
#define nullptr 0//TODO

typedef enum {
	EXPECTED_CR,
	EXPECTED_NEW_LINE,
	BAD_PAYLOAD_TYPE,
	SERVER_ERROR_NULL_PTR,
	SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING,
	EXPECTED_CR_OR_LENGTH_BYTE,
	EXPECTED_LENGTH_BYTE,
	PARSING_FINISHED_CANT_PROCESS_MORE_BYTES,
	INPUT_STATE_WAS_ERROR_STATE,
	BAD_STATE_VALUE,
	CHUNK_SIZE_STRING_BUFFER_FULL,
	INVALID_CHUNK_SIZE_FORMAT,
	HTTP_VERSION_FIRST_NUMBER_BUFFER_FULL,
	HTTP_VERSION_SECOND_NUMBER_BUFFER_FULL,
	HTTP_STATUS_CODE_BUFFER_FULL,
	HTTP_REASON_PHRASE_BUFFER_FULL,
	NO_ERROR,
} HTTPParserErrorState;

static const char *HTTPParserErrorStateErrorStrings[] = {
	"Error: ran into a problem when handling the payload type of the packet (CHUNKED, CONTENT-LENGTH...)",
	"Error: the server got passed a null pointer somewhere during parsing",
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
	"No error, everything is good :)"
};

typedef struct {

} HttpParserCallbackFunction;

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

#endif //__HTTPPARSERCOMMON_H_INCLUDED__
