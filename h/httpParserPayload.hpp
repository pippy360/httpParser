#define MAX_CHUNK_SIZE_STR_BUFFER_LENGTH 40

#include "httpParserCommon.h"

typedef enum{
	CHUNKED_NEW_LINE_R,//\r
	CHUNKED_NEW_LINE_N,//\n
	CHUNKED_DATA_BYTE,//BLOB DATA
	CHUNKED_LENGTH_BYTE,//Chunk length string char, example '9' from "\r\n9999\r\n"
	CHUNKED_INVALID_CHAR
} ChunkedPayloadByteType;

typedef enum {
	PAYLOAD_START,
	CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R,//	\r after length
	CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N,//	\n after length
	CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R,//	\r after data
	CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N,//	\n after data
	CHUNKED_PAYLOAD_LENGTH_CHAR,//while parsing the length of a chunked packet
	CHUNKED_PAYLOAD_DATA,//while parsing the payload data
	CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R,//	\r in \r\n0\r\n\r\n at the end of the encoding
	CHUNKED_PAYLOAD_PACKET_END,//	\n after data
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
