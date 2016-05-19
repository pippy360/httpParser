#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include "../h/httpParserCommon.h"
#include "../h/httpParserHeader.hpp"

int _isNumber(const char nextByte) {
	return nextByte >= '0' && nextByte <= '9';
}

int _isAlphaNumericUppercaseIncluded(const char nextByte) {
	return (nextByte >= '0' && nextByte <= '9')
			|| (nextByte >= 'a' && nextByte <= 'z')
			|| (nextByte >= 'A' && nextByte <= 'Z');
}

int _isValidPathCharacter(const char nextByte) {
	return (nextByte == '/' || _isAlphaNumericUppercaseIncluded(nextByte));
}

//returns 1 if it is A-Z, 0 if false
int _isCapitalLetter(const char nextByte) {
	return (nextByte >= 'A' && nextByte <= 'Z');
}

//return 0 if success, non-0 otherwise
//returns the error type, the error string, returns the byte type
//this is just a reg-ex style check, doesn't handle a lot of cases
//isLastChunkSizeZero needs to be a parameter because we don't parse the value of the size here
//---------------------------------------------------------------
ReturnValueOfGetNextRequestStatusLineStateAndByteType getNextRequestStatusLineStateAndByteType(
		HeaderParserStateEnum stateVal, const char nextByte) {
//---------------------------------------------------------------
	switch (stateVal) {
	case HEADER_REQUEST_START:
		if (_isCapitalLetter(nextByte)) {
			return {HEADER_REQUEST_METHOD, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_METHOD:
		if (_isCapitalLetter(nextByte)) {
			return {HEADER_REQUEST_METHOD, false, NO_ERROR, nullptr};
		} else if (nextByte == ' ') {
			return {HEADER_REQUEST_SPACE_AFTER_METHOD, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_SPACE_AFTER_METHOD:
		if (_isValidPathCharacter(nextByte)) {
			return {HEADER_REQUEST_PATH, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_PATH:
		if (_isValidPathCharacter(nextByte)) { //MUST NOT BE A SPACE (' ') !
			return {HEADER_REQUEST_PATH, false, NO_ERROR, nullptr};
		} else if (nextByte == ' ') {
			return {HEADER_REQUEST_HTTP_VERSION_H, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_H:
		if (nextByte == 'T') {
			return {HEADER_REQUEST_HTTP_VERSION_T, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_T:
		if (nextByte == 'T') {
			return {HEADER_REQUEST_HTTP_VERSION_T_2, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_T_2:
		if (nextByte == 'P') {
			return {HEADER_REQUEST_HTTP_VERSION_P, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_P:
		if (nextByte == '/') {
			return {HEADER_REQUEST_HTTP_VERSION_SLASH, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_SLASH:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR, nullptr};
		} else if (nextByte == '.') {
			return {HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR, nullptr};
		} else if (nextByte == '\r') {
			return {HEADER_REQUEST_NEW_LINE_R, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_NEW_LINE_R:
		if (nextByte == '\n') {
			return {HEADER_REQUEST_FINISHED_PARSING, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_REQUEST_FINISHED_PARSING:
		//error
	case ERROR_HEADER_REQUEST_PARSER:
		return {ERROR_HEADER_REQUEST_PARSER, false, NO_ERROR, nullptr};

	default:
		return {ERROR_HEADER_REQUEST_PARSER, false, NO_ERROR, nullptr};
	}
	//The code never makes it here
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteType",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteType" for performance reasons
//the first few bytes of the packet are handled by bufferAndThenParseFirstFewBytes()
//---------------------------------------------------------------
int headerParserProcessRequestStatusLineByte(HeaderParserState *currState,
		const char nextByte) {
//---------------------------------------------------------------
	if (currState == nullptr) {
		//TODO return a bunch of a error stuff
		return -1;
	}

	return 0;
}

//return 0 if success, non-0 otherwise
//returns the error type, the error string, returns the byte type
//this is just a reg-ex style check, doesn't handle a lot of cases
//isLastChunkSizeZero needs to be a parameter because we don't parse the value of the size here
//---------------------------------------------------------------
ReturnValueOfGetNextResponseStatusLineStateAndByteType getNextResponseStatusLineStateAndByteType(
		HeaderParserStateEnum stateVal, const char nextByte) {
//---------------------------------------------------------------
	switch (stateVal) {
	case HEADER_RESPONSE_START:
		if (nextByte == 'H') {
			return {HEADER_RESPONSE_HTTP_VERSION_H, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_H:
		if (nextByte == 'T') {
			return {HEADER_RESPONSE_HTTP_VERSION_T, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_T:
		if (nextByte == 'T') {
			return {HEADER_RESPONSE_HTTP_VERSION_T_2, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_T_2:
		if (nextByte == 'P') {
			return {HEADER_RESPONSE_HTTP_VERSION_P, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_P:
		if (nextByte == '/') {
			return {HEADER_RESPONSE_HTTP_VERSION_SLASH, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_SLASH:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR, nullptr};
		} else if (nextByte == '.') {
			return {HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR, nullptr};
		} else if (nextByte == ' ') {
			return {HEADER_RESPONSE_SPACE_AFTER_VERSION, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_SPACE_AFTER_VERSION:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_STATUS_CODE:
		//this should probably fource the status code to be 3 chars long
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_STATUS_CODE, false, NO_ERROR, nullptr};
		} else if (nextByte == ' ') {
			return {HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE: //the reason phrase can be empty
	case HEADER_RESPONSE_REASON_PHRASE:
		//we accept extra spaces here!
		if (_isAlphaNumericUppercaseIncluded(nextByte)) {
			return {HEADER_RESPONSE_REASON_PHRASE, false, NO_ERROR, nullptr};
		} else if (nextByte == '\r') {
			return {HEADER_RESPONSE_NEW_LINE_R, false, NO_ERROR, nullptr};
		} else {
			//error
		}
	case HEADER_RESPONSE_NEW_LINE_R:
		if (nextByte == '\n') {
			return {HEADER_RESPONSE_FINISHED_PARSING, false, NO_ERROR, nullptr};
		} else {

		}
	case HEADER_RESPONSE_FINISHED_PARSING:
		//return error
	case ERROR_HEADER_RESPONSE_PARSER:
		//return error
	default:
		//return error
	}
	//The code never makes it here
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteType",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteType" for performance reasons
//the first few bytes of the packet are handled by bufferAndThenParseFirstFewBytes()
//---------------------------------------------------------------
int headerParserProcessResponseStatusLineByte(HeaderParserState *currState,
		const char nextByte) {
//---------------------------------------------------------------
	if (currState == nullptr) {
		//TODO return a bunch of a error stuff
		return -1;
	}

	const ReturnValueOfGetNextResponseStatusLineStateAndByteType retStruct;
	const HeaderParserStateEnum nextStateValue;

	retStruct = getNextResponseStatusLineStateAndByteType(currState->stateVal,
			nextByte);

	currState->stateVal = retStruct.statusLineParserStateEnum;

	if (retStruct.isError) {
		//TODO return a bunch of error stuff
		return -1;
	}

	nextStateValue = retStruct.statusLineParserStateEnum;
	switch (nextStateValue) {
	case HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER:
		//buffer
		break;
	case HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER:

		break;
	case HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER:
		//buffer
		break;
	case HEADER_RESPONSE_SPACE_AFTER_VERSION:

		break;
	case HEADER_RESPONSE_STATUS_CODE:
		//buffer
		break;
	case HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE:

		break;
	case HEADER_RESPONSE_REASON_PHRASE:
		//buffer
		break;
	case HEADER_RESPONSE_NEW_LINE_R:

		break;
	default:
		//TODO error stuff
	}

	return 0;
}

//return 0 if success, non-0 otherwise
//returns the error type, the error string, returns the byte type
//this is just a reg-ex style check, doesn't handle a lot of cases
//isLastChunkSizeZero needs to be a parameter because we don't parse the value of the size here
//---------------------------------------------------------------
ReturnValueOfGetNextInnerHeaderStateAndByteType getNextInnerHeaderStateAndByteType(
		InnerHeadersParserStateEnum stateVal, const char nextByte) {
//---------------------------------------------------------------

	switch (stateVal) {
	case INNER_HEADER_START:
		break;
	case INNER_HEADER_NAME:
		break;
	case INNER_HEADER_COLON:
		break;
	case INNER_HEADER_VALUE:
		break;
	case HEADER_RESPONSE_NEW_LINE_R:
		break;
	case HEADER_RESPONSE_NEW_LINE_N:
		break;
	case HEADER_RESPONSE_FINAL_NEW_LINE_R:
		break;
	case INNER_HEADER_FINISHED_PARSING:
		break;
	case ERROR_INNER_HEADER_PARSER:
		break;
	default:
	}
	//The code never makes it here
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteType",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteType" for performance reasons
//---------------------------------------------------------------
int headerParserProcessInnerHeaderByte(HeaderParserState *currState,
		const char nextByte) {
//---------------------------------------------------------------

	if (currState == nullptr) {
		//TODO return a bunch of a error stuff
		return -1;
	}

	const ReturnValueOfGetNextInnerHeaderStateAndByteType retStruct;
	const HeaderParserStateEnum nextStateValue;

	retStruct = getNextInnerHeaderStateAndByteType(currState->innerHeadersParserState,
			nextByte);

	currState->innerHeadersParserState = retStruct.innerHeadersParserState;

	switch(retStruct.innerHeadersParserState){
	case INNER_HEADER_NAME:
	case INNER_HEADER_COLON:
		//buffer
		break;
	case INNER_HEADER_VALUE:
	case HEADER_RESPONSE_NEW_LINE_R:
		//buffer
		break;
	case INNER_HEADER_FINISHED_PARSING:
		break;
	default:
		break;
	}

	//handle call back functions

	return 0;
}

//returns 0 if success, non-0 otherwise
//---------------------------------------------------------------
HttpRequestMethod httpMethodStringToEnum(const char *methodString) {
//---------------------------------------------------------------
	if (strcmp(methodString, "GET") == 0) {
		return REQUEST_GET;
	} else if (strcmp(methodString, "HEAD") == 0) {
		return REQUEST_HEAD;
	} else if (strcmp(methodString, "POST") == 0) {
		return REQUEST_POST;
	} else if (strcmp(methodString, "PUT") == 0) {
		return REQUEST_PUT;
	} else if (strcmp(methodString, "DELETE") == 0) {
		return REQUEST_DELETE;
	} else if (strcmp(methodString, "TRACE") == 0) {
		return REQUEST_TRACE;
	} else if (strcmp(methodString, "OPTIONS") == 0) {
		return REQUEST_OPTIONS;
	} else if (strcmp(methodString, "CONNECT") == 0) {
		return REQUEST_CONNECT;
	} else if (strcmp(methodString, "PATCH") == 0) {
		return REQUEST_PATCH;
	} else {
		return ERROR_REQUEST_BAD_REQUEST_METHOD;
	}
}

//---------------------------------------------------------------
HeaderParserState httpHeaderParserProcessBuffer(const HeaderParserState state,
		const char *packetBuffer, const int packetBufferLength,
		char **parserBufferEndPtr,
		HttpParserCallbackFunction *callbackFunctions,
		HttpRequestOrResponseType packetType) {
//---------------------------------------------------------------
	HeaderParserState nextState = state;

	int i;

	for (i = 0; i < packetBufferLength; i++) {
		switch (state.stateVal) {
		case HEADER_REQUEST_STATUS_LINE:
			headerParserProcessRequestStatusLineByte(&nextState,
					packetBuffer[i]);
			break;
		case HEADER_RESPONSE_STATUS_LINE:
			headerParserProcessResponseStatusLineByte(&nextState,
					packetBuffer[i]);
			break;
		case HEADER_INNER_HEADERS:
			headerParserProcessInnerHeaderByte(&nextState, packetBuffer[i]);
			break;
		default:
			//TODO error
		}
		//TODO Handle call back functions
	}
	return nextState;
}

//TODO DOCUMENT ME!!!!!!!!!!!!
//---------------------------------------------------------------
HeaderParserState anyHttpPacketHeaderParserParseBuffer(
		const HeaderParserState state, const char *packetBuffer,
		const int packetBufferLength, char **parserBufferEndPtr,
		HttpParserCallbackFunction *callbackFunctions) {
//---------------------------------------------------------------

//Check if we actually need to do any of the below, maybe this isn't a bad a case!
	const HttpRequestOrResponseType packetType;
	const int amountOfCurrentBufferParsed;
	{
		//buffer the first few bytes, and then loop and call the parser
		int i;
		for (i = 0; i < packetBufferLength;
				++i, ++amountOfCurrentBufferParsed) {
			if (_isCapitalLetter (nextByte)) {
				const int length = state.firstFewBytesOfHttpPacketBufferLength;
				if (length + 1 <= MAX_FIRST_FEW_BYTES_LENGTH) {	//+1 because of the '\0'
					state.firstFewBytesOfHttpPacketBuffer[length];
					state.firstFewBytesOfHttpPacketBufferLength++;
				} else {
					//TODO RETURN ERROR BUFFER FULL!!! too many bytes at the start !!!
				}
			} else if (nextByte == '/') {
				packetType = HTTP_PACKET_TYPE_RESPONSE;
			} else if (nextByte == ' ') {
				packetType = HTTP_PACKET_TYPE_REQUEST;
			} else {
				//ERROR BAD CHARACTER
			}
		}
		amountOfCurrentBufferParsed = i;
	}

	const int amountOfCurrentBufferLeftToParse = packetBufferLength
			- amountOfCurrentBufferParsed;
	HeaderParserState nextState = state;

	//if (we exited the loop because we know the packet type and not because we hit the end of the buffer)
	//then (call the parser on the buffer data and the rest of the buffer)
	if (amountOfCurrentBufferParsed != packetBufferLength) {

		if (packetType == HTTP_PACKET_TYPE_REQUEST) {
			//parse the buffered bytes
			nextState = httpHeaderParserProcessBuffer(nextState,
					state.firstFewBytesOfHttpPacketBuffer,
					state.firstFewBytesOfHttpPacketBufferLength,
					parserBufferEndPtr, callbackFunctions,
					HTTP_PACKET_TYPE_REQUEST);

			//then parse the rest of the buffer
			nextState = httpHeaderParserProcessBuffer(nextState,
					packetBuffer + amountOfCurrentBufferParsed,
					amountOfCurrentBufferLeftToParse, parserBufferEndPtr,
					callbackFunctions, HTTP_PACKET_TYPE_REQUEST);

		} else if (packetType == HTTP_PACKET_TYPE_RESPONSE) {
			//parse the buffered bytes
			nextState = httpHeaderParserProcessBuffer(nextState,
					state.firstFewBytesOfHttpPacketBuffer,
					state.firstFewBytesOfHttpPacketBufferLength,
					parserBufferEndPtr, callbackFunctions,
					HTTP_PACKET_TYPE_RESPONSE);

			//then parse the rest of the buffer
			nextState = httpHeaderParserProcessBuffer(nextState,
					packetBuffer + amountOfCurrentBufferParsed,
					amountOfCurrentBufferLeftToParse, parserBufferEndPtr,
					callbackFunctions, HTTP_PACKET_TYPE_RESPONSE);
		}
	}

	return nextState;
}

//3 functions, 1 for request, 1 for response and 1 for unknown

