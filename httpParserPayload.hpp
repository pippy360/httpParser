#define PARSER_ERROR(VALUE) HTTPParserErrorStateErrorStrings[VALUE]
#define MAX_CHUNK_SIZE_STR_BUFFER_LENGTH 40
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

typedef enum{
	NEW_LINE_R,//\r
	NEW_LINE_N,//\n
	DATA_BYTE,//BLOB DATA
	LENGTH_BYTE,//Chunk length string char, example '9' from "\r\n9999\r\n"
	INVALID_CHAR
} ChunkedPayloadByteType;

typedef enum {
	PAYLOAD_START,
	CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R,//	\r after length
	CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N,//	\n after length
	CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R,//	\r after data
	CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N,//	\n after data
	CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R,//	\r in \r\n0\r\n\r\n at the end of the encoding
	CHUNKED_PAYLOAD_PACKET_END,//	\n after data
	CHUNKED_PAYLOAD_LENGTH_CHAR,//while parsing the length of a chunked packet
	CHUNKED_PAYLOAD_DATA,//while parsing the payload data
	ERROR_PAYLOAD_PARSER
} PayloadParserStateEnum;

enum PayloadType{
	CHUNKED_ENCODING_HTTP_PACKET,
	CONTENT_LENGTH_HTTP_PACKET
};

typedef struct {
	PayloadParserStateEnum stateVal;
	PayloadType payloadType;
	long totalBytesProcessedState;
	long remainingBytesInCurrentChunk;
	long currentChunkSize;
	char chunkSizeStrBuffer[MAX_CHUNK_SIZE_STR_BUFFER_LENGTH];
	int chunkSizeStrCurrentLength;
	int isError;
	const char *errorString;
} PayLoadParserState;

/*
static const PayLoadParserState payLoadParserState_init = {
	0,0,0,
}
*/

typedef struct {
	PayloadParserStateEnum payloadParserStateEnum;
	ChunkedPayloadByteType byteType;
	int isError;
	HTTPParserErrorState errorState;
	const char *errorString;
} ReturnValueOfGetNextStateAndByteTypeForChunkedPacket;

typedef struct {

} HttpParserCallbackFunction;
