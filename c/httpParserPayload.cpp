#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include "../h/httpParserCommon.h"
#include "../h/httpParserPayload.hpp"//TODO rename


using namespace std;

int _isChunkedNumber(char byte){
	return ('a' <= byte && byte <= 'e')
		|| ('A' <= byte && byte <= 'E')
		|| ('0' <= byte && byte <= '9');
}

long _chunkSizeStol(const char *str){
	return strtol(str, nullptr, 16);
}

//return 0 if success, non-0 otherwise
//returns the error type, the error string, returns the byte type
//this is just a reg-ex style check, doesn't handle a lot of cases
//isLastChunkSizeZero needs to be a parameter because we don't parse the value of the size here
//---------------------------------------------------------------
ReturnValueOfGetNextStateAndByteTypeForChunkedPacket getNextStateAndByteTypeForChunkedPacket(
	PayloadParserStateEnum stateVal, const char nextByte,
	const long remainingBytesInCurrentChunk){
//---------------------------------------------------------------
	switch (stateVal)
	{
	case PAYLOAD_START:
		if (_isChunkedNumber(nextByte)){
			return{ CHUNKED_PAYLOAD_LENGTH_CHAR, CHUNKED_LENGTH_BYTE, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_LENGTH_BYTE,
				PARSER_ERROR(EXPECTED_LENGTH_BYTE) };
		}
	case CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R:
		if (nextByte == '\n'){
			return{ CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, CHUNKED_NEW_LINE_N, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_CR, PARSER_ERROR(EXPECTED_CR) };
		}
	case CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N:
		if (remainingBytesInCurrentChunk == 0 && nextByte == '\r'){
			return{ CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R, CHUNKED_NEW_LINE_R, false, NO_ERROR, nullptr };
		}else if(remainingBytesInCurrentChunk == 0){
			return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_CR, PARSER_ERROR(EXPECTED_CR) };
		}
		/* no break */
		//continue on to CHUNKED_PAYLOAD_DATA...
	case CHUNKED_PAYLOAD_DATA:
		if (remainingBytesInCurrentChunk > 0){
			return{ CHUNKED_PAYLOAD_DATA, CHUNKED_DATA_BYTE, false, NO_ERROR, nullptr };
		}
		else if (remainingBytesInCurrentChunk == 0){
			if (nextByte == '\r'){
				return{ CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R, CHUNKED_NEW_LINE_R, false, NO_ERROR, nullptr };
			}
			else{
				return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_CR, PARSER_ERROR(EXPECTED_CR) };
			}
		}
		else{
			return { ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true,
				SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING,
				PARSER_ERROR(SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING) };
		}
	case CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R:
		if (nextByte == '\n'){
			return{ CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N, CHUNKED_NEW_LINE_N, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_NEW_LINE,
				PARSER_ERROR(EXPECTED_NEW_LINE) };
		}
	case CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N:
		if (_isChunkedNumber(nextByte)){
			return{ CHUNKED_PAYLOAD_LENGTH_CHAR, CHUNKED_LENGTH_BYTE, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_CR, PARSER_ERROR(EXPECTED_CR) };
		}
	case CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R:
		if (nextByte == '\n'){
			return{ CHUNKED_PAYLOAD_PACKET_END, CHUNKED_NEW_LINE_N, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_NEW_LINE,
				PARSER_ERROR(EXPECTED_NEW_LINE) };
		}
	case CHUNKED_PAYLOAD_LENGTH_CHAR:
		if (_isChunkedNumber(nextByte)){
			return{ CHUNKED_PAYLOAD_LENGTH_CHAR, CHUNKED_LENGTH_BYTE, false, NO_ERROR, nullptr };
		}
		else if (nextByte == '\r'){
			return{ CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R, CHUNKED_NEW_LINE_R, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, EXPECTED_CR_OR_LENGTH_BYTE, PARSER_ERROR(EXPECTED_CR) };
		}
	case CHUNKED_PAYLOAD_PACKET_END:
		return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, PARSING_FINISHED_CANT_PROCESS_MORE_BYTES,
			PARSER_ERROR(PARSING_FINISHED_CANT_PROCESS_MORE_BYTES) };
	case ERROR_PAYLOAD_PARSER:
		return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, INPUT_STATE_WAS_ERROR_STATE,
			PARSER_ERROR(INPUT_STATE_WAS_ERROR_STATE) };
	default:
		//throw an exception
		return{ ERROR_PAYLOAD_PARSER, CHUNKED_INVALID_CHAR, true, BAD_STATE_VALUE, PARSER_ERROR(BAD_STATE_VALUE) };
	}
	//The code never makes it here
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteTypeForChunkedPacket",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteTypeForChunkedPacket" for performance reasons
//---------------------------------------------------------------
int chunkedPayloadParserProcessByte(PayLoadParserState *currState, const char nextByte){
//---------------------------------------------------------------
	if (currState == nullptr){
		//TODO return a bunch of a error stuff
		return -1;
	}

	const ReturnValueOfGetNextStateAndByteTypeForChunkedPacket retStruct = getNextStateAndByteTypeForChunkedPacket(currState->stateVal,
			nextByte, currState->currentChunkSize);

	currState->stateVal = retStruct.payloadParserStateEnum;

	if (retStruct.isError){
		//TODO return a bunch of error stuff
		return -1;
	}

	const PayloadParserStateEnum nextStateValue = retStruct.payloadParserStateEnum;
	switch(nextStateValue){
	case CHUNKED_PAYLOAD_LENGTH_CHAR:
		if (currState->chunkSizeStrCurrentLength + 2 < MAX_CHUNK_SIZE_STR_BUFFER_LENGTH){//+2 for new char and '\0'
			//strcpy_s(currState->chunkSizeStrBuffer, &nextByte);
			strcpy(currState->chunkSizeStrBuffer, &nextByte);
			currState->chunkSizeStrCurrentLength++;
		}
		else{
			//TODO return a bunch of error stuff
			//buffer error stuff here
			return -1;
		}
		break;
	case CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R:
		//TODO check for rollover
		//TODO check for negative value
		//TODO check all the buffers!
		currState->currentChunkSize = _chunkSizeStol(currState->chunkSizeStrBuffer);
		break;
	default:
		//TODO error stuff
		;
	}

	return 0;
}

//---------------------------------------------------------------
PayLoadParserState chunkedPayloadParserProcessBuffer(
		const PayLoadParserState state, const char *packetBuffer,
		const int packetBufferLength,
		const HttpParserCallbackFunction *callbackFunctions)
//---------------------------------------------------------------
{
	PayLoadParserState nextState = state;
	int i;
	for (i = 0; i < packetBufferLength; i++){
		chunkedPayloadParserProcessByte(&nextState, packetBuffer[i]);
		//check for erros
		//callbackFunctions
	}
	return nextState;
}


//---------------------------------------------------------------
PayLoadParserState contentLengthPayloadParserProcessBuffer(
		const PayLoadParserState state, const char *packetBuffer,
		const int packetBufferLength,
		const HttpParserCallbackFunction *callbackFunctions) {
//---------------------------------------------------------------
	PayLoadParserState nextState = state;
	int i;
	for (i = 0; i < packetBufferLength; i++){
		chunkedPayloadParserProcessByte(&nextState, packetBuffer[i]);
		//check for errors
		//callbackfunctions
	}
	return nextState;
}

//parserBufferEndPtr is
//---------------------------------------------------------------
PayLoadParserState httpPayloadParserProcessBuffer(
		const PayLoadParserState state, const char *packetBuffer,
		const int packetBufferLength, char **parserBufferEndPtr,
		HttpParserCallbackFunction *callbackFunctions) {
//---------------------------------------------------------------
	if (state.payloadType == CHUNKED_ENCODING_HTTP_PACKET){
		return chunkedPayloadParserProcessBuffer(state, packetBuffer, packetBufferLength, nullptr);
	}
	else if (state.payloadType == CONTENT_LENGTH_HTTP_PACKET){
		return contentLengthPayloadParserProcessBuffer(state, packetBuffer, packetBufferLength, nullptr);
	}
	else{
		//TODO set the error state;
		//REMOVE THIS LINE:
		return contentLengthPayloadParserProcessBuffer(state, packetBuffer, packetBufferLength, nullptr);
		//return ;
	}
}
