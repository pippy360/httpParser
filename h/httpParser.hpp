#include "httpParserCommon.h"

typedef enum {
	HEADER_PARSER_STATUS_LINE,
	HEADER_PARSER_INNER_HEADERS
} CurrentActiveParser;

typedef struct {
	HttpPacketInfo httpInfo;
	//Info for the sub-parsers
	long totalBytesProcessedState;
	CurrentActiveParser currentActiveParser;
	HeaderParserState headerParserState;
	PayloadParserState payloadState;
	//errors
	int isError;
	HTTPParserErrorState errorState;
} HttpParserState;
