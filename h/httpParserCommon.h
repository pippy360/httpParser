#ifndef __HTTPPARSERCOMMON_H_INCLUDED__
#define __HTTPPARSERCOMMON_H_INCLUDED__

#include "httpCommon.h"

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
	FAILED_TO_PARSE_REQUEST_STATUS_LINE,
	FAILED_TO_PARSE_RESPONSE_STATUS_LINE,
	FAILED_TO_PARSE_INNER_HEADERS,
	HTTP_INNER_HEADER_NAME_BUFFER_FULL,
	HTTP_INNER_HEADER_VALUE_BUFFER_FULL,
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

#endif //__HTTPPARSERCOMMON_H_INCLUDED__










