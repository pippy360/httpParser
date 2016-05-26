#ifndef __HTTPPARSERPAYLOAD_H_INCLUDED__
#define __HTTPPARSERPAYLOAD_H_INCLUDED__

#define MAX_CHUNK_SIZE_STR_BUFFER_LENGTH 40

#include "httpCommon.h"
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
} ChunkedPayloadParserStateEnum;

typedef struct {
	ChunkedPayloadParserStateEnum chunkedStateEnum;
	char chunkSizeStrBuffer[MAX_CHUNK_SIZE_STR_BUFFER_LENGTH];
	int chunkSizeStrLength;
	//const int maxSizeOfChunkSizeStrBuffer = MAX_CHUNK_SIZE_STR_BUFFER_LENGTH;FIXME
	long currentChunkSize;
	long remainingBytesInCurrentChunk;
	//errors
	int isError;
	HTTPParserErrorState errorState;
} PayloadParserStateChunkedPayloadState;

typedef enum {
	CHUNKED_ENCODING_HTTP_PACKET_PARSER,
	CONTENT_LENGTH_HTTP_PACKET_PARSER,
} CurrentActiveParserForPayloadType;

typedef struct {

} PayloadParserStateContentLengthPayloadState;

typedef struct {
	CurrentActiveParserForPayloadType payloadParser;
	char *dataStart;
	char *dataEnd;
	int dataLength;
	//subparsers
	PayloadParserStateChunkedPayloadState chunkedParserState;
	PayloadParserStateContentLengthPayloadState contentLengthParserState;
	//errors
	int isError;
	HTTPParserErrorState errorState;
} PayloadParserState;

/*
static const PayLoadParserState payLoadParserState_init = {
	0,0,0,
}
*/

typedef struct {
	ChunkedPayloadParserStateEnum chunkedStateEnum;
	ChunkedPayloadByteType byteType;
	int isError;
	HTTPParserErrorState errorState;
} ReturnValueOfGetNextStateAndByteTypeForChunkedPacket;


#endif //__HTTPPARSERPAYLOAD_H_INCLUDED__
