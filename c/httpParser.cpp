#include "../h/httpParserCommon.h"
#include "../h/httpParser.hpp"
#include "../h/httpParserPayload.hpp"

//returns 0 if success, non-0 otherwise
int parseStatusLineWrapper(ParserOverviewState *state, const char *buffer,
		const int bufferLength, char **bufferEndPtr) {
	//strip the state stuff we actually want
	//call another function after the stripping _internal
	//increment the bytes processed
}

//returns 0 if success, non-0 otherwise
int parseHeadersWrapper(ParserOverviewState *state, const char *buffer,
		const int bufferLength, char **bufferEndPtr) {
	//strip the state stuff we actually want
	//call another function after the stripping _internal
	//increment the bytes processed
}

//returns 0 if success, non-0 otherwise
int parsePayloadWrapper(ParserOverviewState *state, const char *buffer,
		const int bufferLength, char **bufferEndPtr) {

	HttpParserCallbackFunction *callbackFunctions = &(state->callbackFunctions);//TODO will this be passed by value or reference?
	PayLoadParserState payloadParserState = state->payloadParserState;
	httpPayloadParserProcessBuffer(payloadParserState, buffer,
			bufferLength, bufferEndPtr, callbackFunctions);
}

//returns 0 if success, non-0 otherwise
ParserOverviewState parseHttpPacketBuffer(const ParserOverviewState *state,
		const char *buffer, const int bufferLength) {

	if (state == nullptr){
		//TODO handle error
		return -1;
	}

	ParserOverviewState nextState = *state;
	int currentBufferBytesProcessed = 0;
	const char *bufferEndPosition = buffer + bufferLength;

	char *currBufferPos = buffer;
	while (currBufferPos < bufferEndPosition && state->stateEnum != HTTP_PACKET_FINISHED){
		int parserErrorFlag = 0;
		int remainingBufferLength = bufferLength - currentBufferBytesProcessed;
		char *parserEndPostion;

		switch (state->stateEnum) {
		case HTTP_PACKET_STATUS_LINE:
			parserErrorFlag = parseStatusLineWrapper(&nextState, currBufferPos,
					remainingBufferLength, &parserEndPostion);
			break;
		case HTTP_PACKET_HEADERS:
			parserErrorFlag = parseHeadersWrapper(&nextState, currBufferPos,
					remainingBufferLength, &parserEndPostion);
			break;
		case HTTP_PACKET_PAYLOAD:
			parserErrorFlag = parsePayloadWrapper(&nextState, currBufferPos,
					remainingBufferLength, &parserEndPostion);
			break;
		}
		currBufferPos = parserEndPostion;

		if (parserErrorFlag != 0){
			//TODO handle error
			return -1;
		}

	}

	return nextState;
}
