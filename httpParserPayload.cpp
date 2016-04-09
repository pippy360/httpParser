#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include "httpParserCommon.h"//TODO rename
#include "httpParserPayload.hpp"//TODO rename


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
ReturnValueOfGetNextStateAndByteTypeForChunkedPacket getNextStateAndByteTypeForChunkedPacket(
	PayloadParserStateEnum stateVal, const char nextByte,
	const long remainingBytesInCurrentChunk, const int isLastChunkSizeZero){

	switch (stateVal)
	{
	case PAYLOAD_START:
		if (_isChunkedNumber(nextByte)){
			return{ CHUNKED_PAYLOAD_LENGTH_CHAR, LENGTH_BYTE, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, EXPECTED_LENGTH_BYTE,
				PARSER_ERROR(EXPECTED_LENGTH_BYTE) };
		}
	case CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R:
		if (nextByte == '\n'){
			return{ CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, NEW_LINE_N, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, EXPECTED_CR, PARSER_ERROR(EXPECTED_CR) };
		}
	case CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N:
		if (isLastChunkSizeZero && nextByte == '\r'){
			return{ CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R, NEW_LINE_R, false, NO_ERROR, nullptr };
		}
		/* no break */
		//continue on to CHUNKED_PAYLOAD_DATA...
	case CHUNKED_PAYLOAD_DATA:
		if (remainingBytesInCurrentChunk > 0){
			return{ CHUNKED_PAYLOAD_DATA, DATA_BYTE, false, NO_ERROR, nullptr };
		}
		else if (remainingBytesInCurrentChunk == 0){
			if (nextByte == '\r'){
				return{ CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R, NEW_LINE_R, false, NO_ERROR, nullptr };
			}
			else{
				return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, EXPECTED_CR, PARSER_ERROR(EXPECTED_CR) };
			}
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, false,
				SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING,
				PARSER_ERROR(SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING) };
		}
	case CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R:
		if (nextByte == '\n'){
			return{ CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N, NEW_LINE_N, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, EXPECTED_NEW_LINE,
				PARSER_ERROR(EXPECTED_NEW_LINE) };
		}
	case CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N:
		if (_isChunkedNumber(nextByte)){
			return{ CHUNKED_PAYLOAD_LENGTH_CHAR, LENGTH_BYTE, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, EXPECTED_CR, PARSER_ERROR(EXPECTED_CR) };
		}
	case CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R:
		if (nextByte == '\n'){
			return{ CHUNKED_PAYLOAD_PACKET_END, NEW_LINE_N, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, EXPECTED_NEW_LINE,
				PARSER_ERROR(EXPECTED_NEW_LINE) };
		}
	case CHUNKED_PAYLOAD_LENGTH_CHAR:
		if (_isChunkedNumber(nextByte)){
			return{ CHUNKED_PAYLOAD_LENGTH_CHAR, LENGTH_BYTE, false, NO_ERROR, nullptr };
		}
		else if (nextByte == '\r'){
			return{ CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R, NEW_LINE_R, false, NO_ERROR, nullptr };
		}
		else{
			return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, EXPECTED_CR_OR_LENGTH_BYTE, PARSER_ERROR(EXPECTED_CR) };
		}
	case CHUNKED_PAYLOAD_PACKET_END:
		return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, false, PARSING_FINISHED_CANT_PROCESS_MORE_BYTES,
			PARSER_ERROR(PARSING_FINISHED_CANT_PROCESS_MORE_BYTES) };
	case ERROR_PAYLOAD_PARSER:
		return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, false, INPUT_STATE_WAS_ERROR_STATE,
			PARSER_ERROR(INPUT_STATE_WAS_ERROR_STATE) };
	default:
		//throw an exception
		return{ ERROR_PAYLOAD_PARSER, INVALID_CHAR, true, BAD_STATE_VALUE, PARSER_ERROR(BAD_STATE_VALUE) };
	}
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteTypeForChunkedPacket",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteTypeForChunkedPacket" for performance reasons
int chunkedPayloadProcessByte(PayLoadParserState *currState, const char nextByte){
	ReturnValueOfGetNextStateAndByteTypeForChunkedPacket retStruct;
	ChunkedPayloadByteType byteType;
	PayloadParserStateEnum nextStateValue;

	if (currState == nullptr){
		//TODO return a bunch of a error stuff
		return -1;
	}

	int isChunkSizeZero = (currState->currentChunkSize == 0);
	retStruct = getNextStateAndByteTypeForChunkedPacket(currState->stateVal, nextByte, currState->currentChunkSize,
		isChunkSizeZero);
	currState->stateVal = retStruct.payloadParserStateEnum;
	byteType = retStruct.byteType;
	nextStateValue = retStruct.payloadParserStateEnum;

	if (retStruct.isError){
		//TODO return a bunch of error stuff
		return -1;
	}

	if (nextStateValue == CHUNKED_PAYLOAD_LENGTH_CHAR){
		if (currState->chunkSizeStrCurrentLength + 2 < MAX_CHUNK_SIZE_STR_BUFFER_LENGTH){//+2 for new char and '\0'
			//strcpy_s(currState->chunkSizeStrBuffer, &nextByte);
			strcpy(currState->chunkSizeStrBuffer, &nextByte);
			currState->chunkSizeStrCurrentLength++;
		}
		else{
			//TODO return a bunch of error stuff
			return -1;
		}
	}else if(nextStateValue == CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R){
		currState->currentChunkSize == _chunkSizeStol(currState->chunkSizeStrBuffer);
	}

	return 0;
}


PayLoadParserState chunkedPayloadParserProcessBuffer(const PayLoadParserState state, const char *packetBuffer, const int packetBufferLength, const HttpParserCallbackFunction *callbackFunctions)
{
	PayLoadParserState nextState = state;
	int i;
	for (i = 0; i < packetBufferLength; i++){
		chunkedPayloadProcessByte(&nextState, packetBuffer[i]);
		//check for erros
		//callbackFunctions
	}
	return nextState;
}

PayLoadParserState contentLengthPayloadParserProcessBuffer(const PayLoadParserState state, const char *packetBuffer, const int packetBufferLength, const HttpParserCallbackFunction *callbackFunctions){
	PayLoadParserState nextState = state;
	int i;
	for (i = 0; i < packetBufferLength; i++){
		chunkedPayloadProcessByte(&nextState, packetBuffer[i]);
		//check for erros
		//callbackfunctions
	}
	return nextState;
}

PayLoadParserState httpPayloadParserProcessBuffer(const PayLoadParserState state, const char *packetBuffer, const int packetBufferLength)
{
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

//handle all the mini parsers
//returns a completely new state each time! no, it can't?
//can handle a partial buffer
//should this know about the partial buffer?
void parseHttpPacketBuffer(const PayLoadParserState state, const char *packetBuffer, int packetBufferLength){
	//clear error each time ??

	//hm....do this until we finish parsing the buffer
	while (/*were not at the end of the packet, and the state hasn't hit end*/ true){
		/*
		if (parserStage == enum_StatusLine){
		//parse the statusLine
		}
		else if (parserStage == httpHeader){
		//parse the header
		}
		else if (parserStage == httpPayload){
		//parse the payload
		}
		else if (parserStage == error){
		//handle error
		}
		else if (parserStage == end){
		//do nothing
		}
		else{
		//invalid parser stage
		}
		*/
	}
}

void testChunk(const char *chunk, long chunkSize){
	ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret;
	PayloadParserStateEnum nextStateVal = CHUNKED_PAYLOAD_LENGTH_CHAR;

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, chunk[0], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_R){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, chunk[1], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_N){
		//fail()
		printf("we got an error :( 5\n");
	}

	for (size_t i = 0; i < chunkSize; i++)
	{
		ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, chunk[2 + i], chunkSize - i, false);
		nextStateVal = ret.payloadParserStateEnum;
		if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_DATA){
			//fail()
			printf("we got an error :( 5\n");
		}
		if (ret.isError != false){
			//fail()
			printf("we got an error :( 5\n");
		}
		if (ret.byteType != DATA_BYTE){
			//fail()
			printf("we got an error :( 5\n");
		}
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, chunk[2 + chunkSize], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_R){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, chunk[3 + chunkSize], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_N){
		//fail()
		printf("we got an error :( 5\n");
	}
}

PayLoadParserState testStructState = {
	PAYLOAD_START,
	CHUNKED_ENCODING_HTTP_PACKET,
	0,
	0,
	0,
	"\0",
	false,
	0,
	nullptr
};

const char *testPkt = "4\r\nWiki\r\n5\r\npedia\r\nE\r\nin\r\n\r\nchunks. \r\n11\r\n12345678901234567\r\n0\r\n\r\n";
size_t testPktLen = std::strlen(testPkt);
void getNextStateAndByteTypeForChunkedPacket_test_trivial_case_valid_packet(){
	//test all the states manually
	PayLoadParserState tempState = testStructState;
	ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret;
	PayloadParserStateEnum nextStateVal = testStructState.stateVal;

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[0], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_CHAR){
		//fail()
		printf("we got an error :( 2\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != LENGTH_BYTE){
		//fail()
		printf("we got an error :( 5\n");
	}

	testChunk(testPkt + 1, 4);

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[9], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_CHAR){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != LENGTH_BYTE){
		//fail()
		printf("we got an error :( 5\n");
	}

	testChunk(testPkt + 10, 5);

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[19], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_CHAR){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != LENGTH_BYTE){
		//fail()
		printf("we got an error :( 5\n");
	}

	testChunk(testPkt + 20, 14);

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[38], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_CHAR){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != LENGTH_BYTE){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[39], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_CHAR){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != LENGTH_BYTE){
		//fail()
		printf("we got an error :( 5\n");
	}

	testChunk(testPkt + 40, 17);

	//now handle the zero chunk
	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[61], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_CHAR){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != LENGTH_BYTE){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[62], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_R){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[63], 0, false);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N){
		//fail()
		printf("we got an error :( 4\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_N){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[64], 0, true);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_R){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal, testPkt[65], 0, true);
	nextStateVal = ret.payloadParserStateEnum;
	if (ret.payloadParserStateEnum != CHUNKED_PAYLOAD_PACKET_END){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.isError != false){
		//fail()
		printf("we got an error :( 5\n");
	}
	if (ret.byteType != NEW_LINE_N){
		//fail()
		printf("we got an error :( 5\n");
	}

}

void getNextStateAndByteTypeForChunkedPacket_test_all_possible_errors(){
	//TODO:
}

void chunkedPayloadParserProcessBuffer_test_simple() {
	PayLoadParserState stateStruct = testStructState;
	char *packet = "3\r\nthi\r\ns is a packet";
	chunkedPayloadParserProcessBuffer(stateStruct, packet, strlen(packet), nullptr);

	//trivial test

	//test the buffer overflows tests too

	//test
}

void main(){

	getNextStateAndByteTypeForChunkedPacket_test_trivial_case_valid_packet();
	chunkedPayloadParserProcessBuffer_test_simple();
	//test the error states
	{
		//test 5\r\n\r\n !that should work !!
		//test ...someData\r\n\r\n //that should NOT work
		//test 0\r\ntest\r\n
	}

	printf("done!\n");
	return;
}
