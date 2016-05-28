#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include "../h/utils.h"
#include "../h/httpParserCommonUtils.h"
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
	//TODO rewrite, you should use a constant time lookup
	char validChars[] = { '/', '\\', '-', '.', '_', '~', '!', '$', '&', '\'',
			'(', ')', '*', '+', ',', ';', '=', ':', '@', '%' };

	if (_isAlphaNumericUppercaseIncluded(nextByte)) {
		return true;
	}

	for (int i = 0; i < 20; i++) {
		if (nextByte == validChars[i]) {
			return true;
		}
	}
	return false;
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
		StatusLineRequestHeaderParserStateEnum stateVal, const char nextByte) {
//---------------------------------------------------------------
	const ReturnValueOfGetNextRequestStatusLineStateAndByteType basicErroStruct =
			{ ERROR_HEADER_REQUEST_PARSER, true,
					FAILED_TO_PARSE_REQUEST_STATUS_LINE };
	switch (stateVal) {
	case HEADER_REQUEST_START:
		if (_isCapitalLetter(nextByte)) {
			return {HEADER_REQUEST_METHOD, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_METHOD:
		if (_isCapitalLetter(nextByte)) {
			return {HEADER_REQUEST_METHOD, false, NO_ERROR};
		} else if (nextByte == ' ') {
			return {HEADER_REQUEST_SPACE_AFTER_METHOD, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_SPACE_AFTER_METHOD:
		if (_isValidPathCharacter(nextByte)) {
			return {HEADER_REQUEST_PATH, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_PATH:
		if (_isValidPathCharacter(nextByte)) { //MUST NOT BE A SPACE (' ') !
			return {HEADER_REQUEST_PATH, false, NO_ERROR};
		} else if (nextByte == ' ') {
			return {HEADER_SPACE_AFTER_PATH, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_SPACE_AFTER_PATH:
		if (nextByte == 'H') {
			return {HEADER_REQUEST_HTTP_VERSION_H, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_H:
		if (nextByte == 'T') {
			return {HEADER_REQUEST_HTTP_VERSION_T, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_T:
		if (nextByte == 'T') {
			return {HEADER_REQUEST_HTTP_VERSION_T_2, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_T_2:
		if (nextByte == 'P') {
			return {HEADER_REQUEST_HTTP_VERSION_P, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_P:
		if (nextByte == '/') {
			return {HEADER_REQUEST_HTTP_VERSION_SLASH, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_SLASH:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR};
		} else if (nextByte == '.') {
			return {HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR};
		} else if (nextByte == '\r') {
			return {HEADER_REQUEST_NEW_LINE_R, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_NEW_LINE_R:
		if (nextByte == '\n') {
			return {HEADER_REQUEST_FINISHED_PARSING, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_REQUEST_FINISHED_PARSING:
		return basicErroStruct;
	case ERROR_HEADER_REQUEST_PARSER:
		return basicErroStruct;

	default:
		return basicErroStruct;
	}
	//The code never makes it here
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteType",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteType" for performance reasons
//the first few bytes of the packet are handled by bufferAndThenParseFirstFewBytes()
//---------------------------------------------------------------
int headerParserProcessRequestStatusLineByte(
		HeaderParserStatusLineParserStateRequest *currState,
		const char nextByte) {
//---------------------------------------------------------------
	if (currState == nullptr) {
		//TODO return a bunch of a error stuff
		return -1;
	}

	const ReturnValueOfGetNextRequestStatusLineStateAndByteType retStruct =
			getNextRequestStatusLineStateAndByteType(currState->stateVal,
					nextByte);

	currState->stateVal = retStruct.statusLineParserStateEnum;
	currState->isError = retStruct.isError;
	currState->errorState = retStruct.errorState;

	if (retStruct.isError) {
		return -1;
	}

	const StatusLineRequestHeaderParserStateEnum nextStateValue =
			retStruct.statusLineParserStateEnum;
	switch (nextStateValue) {
	case HEADER_REQUEST_METHOD:
	case HEADER_REQUEST_SPACE_AFTER_METHOD:
	case HEADER_REQUEST_PATH:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(
				currState->httpVersionFirstNumberBuffer,
				&(currState->httpVersionFirstNumberBufferLength),
				HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH,
				nextByte) != 0) {
			currState->isError = true;
			currState->errorState = HTTP_VERSION_FIRST_NUMBER_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_SPACE_AFTER_PATH:
		//convert
	case HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(
				currState->httpVersionFirstNumberBuffer,
				&(currState->httpVersionFirstNumberBufferLength),
				HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH,
				nextByte) != 0) {
			currState->isError = true;
			currState->errorState = HTTP_VERSION_FIRST_NUMBER_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER:
		//convert
		break;
	case HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(
				currState->httpVersionSecondNumberBuffer,
				&(currState->httpVersionSecondNumberBufferLength),
				HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH,
				nextByte) != 0) {
			currState->isError = true;
			currState->errorState = HTTP_VERSION_SECOND_NUMBER_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_REQUEST_NEW_LINE_R:
		//convert
		printf("the http version is: http/%s.%s\n",
				currState->httpVersionFirstNumberBuffer,
				currState->httpVersionSecondNumberBuffer);
		break;
	default:
		//TODO error stuff
		;
	}

	return 0;
}

//return 0 if success, non-0 otherwise
//returns the error type, the error string, returns the byte type
//this is just a reg-ex style check, doesn't handle a lot of cases
//isLastChunkSizeZero needs to be a parameter because we don't parse the value of the size here
//---------------------------------------------------------------
ReturnValueOfGetNextResponseStatusLineStateAndByteType getNextResponseStatusLineStateAndByteType(
		StatusLineResponseHeaderParserStateEnum stateVal, const char nextByte) {
//---------------------------------------------------------------
	const ReturnValueOfGetNextResponseStatusLineStateAndByteType basicErroStruct =
			{ ERROR_HEADER_RESPONSE_PARSER, true,
					FAILED_TO_PARSE_RESPONSE_STATUS_LINE };
	switch (stateVal) {
	case HEADER_RESPONSE_START:
		if (nextByte == 'H') {
			return {HEADER_RESPONSE_HTTP_VERSION_H, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_H:
		if (nextByte == 'T') {
			return {HEADER_RESPONSE_HTTP_VERSION_T, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_T:
		if (nextByte == 'T') {
			return {HEADER_RESPONSE_HTTP_VERSION_T_2, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_T_2:
		if (nextByte == 'P') {
			return {HEADER_RESPONSE_HTTP_VERSION_P, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_P:
		if (nextByte == '/') {
			return {HEADER_RESPONSE_HTTP_VERSION_SLASH, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_SLASH:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER, false, NO_ERROR};
		} else if (nextByte == '.') {
			return {HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER, false, NO_ERROR};
		} else if (nextByte == ' ') {
			return {HEADER_RESPONSE_SPACE_AFTER_VERSION, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_SPACE_AFTER_VERSION:
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_STATUS_CODE, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_STATUS_CODE:
		//this should probably fource the status code to be 3 chars long
		if (_isNumber(nextByte)) {
			return {HEADER_RESPONSE_STATUS_CODE, false, NO_ERROR};
		} else if (nextByte == ' ') {
			return {HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE: //the reason phrase can be empty
	case HEADER_RESPONSE_REASON_PHRASE:
		//we accept extra spaces here!
		if (_isAlphaNumericUppercaseIncluded(nextByte)) {
			return {HEADER_RESPONSE_REASON_PHRASE, false, NO_ERROR};
		} else if (nextByte == '\r') {
			return {HEADER_RESPONSE_NEW_LINE_R, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_NEW_LINE_R:
		if (nextByte == '\n') {
			return {HEADER_RESPONSE_FINISHED_PARSING, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case HEADER_RESPONSE_FINISHED_PARSING:
		return basicErroStruct;
	case ERROR_HEADER_RESPONSE_PARSER:
		return basicErroStruct;
	default:
		return basicErroStruct;
	}
	//The code never makes it here
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteType",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteType" for performance reasons
//the first few bytes of the packet are handled by bufferAndThenParseFirstFewBytes()
//---------------------------------------------------------------
int headerParserProcessResponseStatusLineByte(
		HeaderParserStatusLineParserStateResponse *currState,
		const char nextByte) {
//---------------------------------------------------------------
	if (currState == nullptr) {
		//TODO return a bunch of a error stuff
		return -1;
	}

	const ReturnValueOfGetNextResponseStatusLineStateAndByteType retStruct =
			getNextResponseStatusLineStateAndByteType(currState->stateVal,
					nextByte);

	currState->stateVal = retStruct.statusLineParserStateEnum;
	currState->isError = retStruct.isError;
	currState->errorState = retStruct.errorState;

	if (retStruct.isError) {
		return -1;
	}

	const StatusLineResponseHeaderParserStateEnum nextStateValue =
			retStruct.statusLineParserStateEnum;
	switch (nextStateValue) {
	case HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(
				currState->httpVersionFirstNumberBuffer,
				&(currState->httpVersionFirstNumberBufferLength),
				HTTP_HEADER_PARSER_MAX_HTTP_FIRST_NUMBER_BUFFER_LENGTH,
				nextByte) != 0) {
			currState->isError = true;
			currState->errorState = HTTP_VERSION_FIRST_NUMBER_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER:
		//convert
		break;
	case HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(
				currState->httpVersionSecondNumberBuffer,
				&(currState->httpVersionSecondNumberBufferLength),
				HTTP_HEADER_PARSER_MAX_HTTP_SECOND_NUMBER_BUFFER_LENGTH,
				nextByte) != 0) {
			currState->isError = true;
			currState->errorState = HTTP_VERSION_SECOND_NUMBER_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_RESPONSE_SPACE_AFTER_VERSION:
		//convert
		printf("the http version is: http/%s.%s\n",
				currState->httpVersionFirstNumberBuffer,
				currState->httpVersionSecondNumberBuffer);
		break;
	case HEADER_RESPONSE_STATUS_CODE:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(
				currState->httpStatusCodeBuffer,
				&(currState->httpStatusCodeBufferLength),
				HTTP_HEADER_PARSER_MAX_STATUS_CODE_BUFFER_LENGTH, nextByte)
				!= 0) {
			currState->isError = true;
			currState->errorState = HTTP_STATUS_CODE_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE:
		printf("the status code is: %s\n", currState->httpStatusCodeBuffer);
		break;
	case HEADER_RESPONSE_REASON_PHRASE:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(
				currState->httpReasonPhraseBuffer,
				&(currState->httpReasonPhraseBufferLength),
				HTTP_HEADER_PARSER_MAX_REASON_PHRASE_BUFFER_LENGTH, nextByte)
				!= 0) {
			currState->isError = true;
			currState->errorState = HTTP_REASON_PHRASE_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_RESPONSE_NEW_LINE_R:
		//do nothing
		break;
	default:
		//TODO error stuff
		;
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
	const ReturnValueOfGetNextInnerHeaderStateAndByteType basicErroStruct = {
			ERROR_INNER_HEADER_PARSER, true, FAILED_TO_PARSE_INNER_HEADERS };
	switch (stateVal) {
	case INNER_HEADER_START:
		if (_isAlphaNumericUppercaseIncluded(nextByte)) {
			return {INNER_HEADER_NAME, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case INNER_HEADER_NAME:
		if (nextByte == ':') {
			return {INNER_HEADER_COLON, false, NO_ERROR};
		} else if (_isAlphaNumericUppercaseIncluded(nextByte)) {
			return {INNER_HEADER_NAME, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case INNER_HEADER_COLON:
		//allow empty values
	case INNER_HEADER_VALUE:
		if (nextByte == '\r') {
			return {INNER_HEADER_NEW_LINE_R, false, NO_ERROR};
		} else if (_isAlphaNumericUppercaseIncluded(nextByte)
				|| nextByte == ' ') { //TODO || nextByte == ' ' shouldn't be there!
			return {INNER_HEADER_VALUE, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case INNER_HEADER_NEW_LINE_R:
		if (nextByte == '\n') {
			return {INNER_HEADER_NEW_LINE_N, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case INNER_HEADER_NEW_LINE_N:
		if (nextByte == '\r') {
			return {INNER_HEADER_FINAL_NEW_LINE_R, false, NO_ERROR};
		} else if (_isAlphaNumericUppercaseIncluded(nextByte)) {
			return {INNER_HEADER_NAME, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case INNER_HEADER_FINAL_NEW_LINE_R:
		if (nextByte == '\n') {
			return {INNER_HEADER_FINISHED_PARSING, false, NO_ERROR};
		} else {
			return basicErroStruct;
		}
	case INNER_HEADER_FINISHED_PARSING:
		return basicErroStruct;
	case ERROR_INNER_HEADER_PARSER:
		return basicErroStruct;
	default:
		return basicErroStruct;
	}
	//The code never makes it here
}

//returns 0 if success, non-0 otherwise
//as much logic as possible has been extracted to the functional/pure function "getNextStateAndByteType",
//this function acts like a wrapper for that function, it only handles some logic that isn't
//handled in "getNextStateAndByteType" for performance reasons
//---------------------------------------------------------------
int headerParserProcessInnerHeaderByte(InnerHeadersParserState *currState,
		const char nextByte) {
//---------------------------------------------------------------

	if (currState == nullptr) {
		//TODO return a bunch of a error stuff
		return -1;
	}

	//const HeaderParserStateEnum nextStateValue;
	const ReturnValueOfGetNextInnerHeaderStateAndByteType retStruct =
			getNextInnerHeaderStateAndByteType(currState->stateVal, nextByte);

	currState->stateVal = retStruct.innerHeadersParserStateEnum;
	currState->isError = retStruct.isError;
	currState->errorState = retStruct.errorState;

	if (retStruct.isError) {
		return -1;
	}

	switch (retStruct.innerHeadersParserStateEnum) {
	case INNER_HEADER_NAME:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(currState->innerHeaderName,
				&(currState->innerHeaderNameLength),
				MAX_SIZE_OF_INNER_HEADER_NAME, nextByte) != 0) {
			currState->isError = true;
			currState->errorState = HTTP_INNER_HEADER_NAME_BUFFER_FULL;
			return -1;
		}
		break;
	case INNER_HEADER_COLON:
		//convert
	case INNER_HEADER_VALUE:
		//buffer
		if (_addToHttpStateBufferWithErrorChecking(currState->innerHeaderValue,
				&(currState->innerHeaderValueLength),
				MAX_SIZE_OF_INNER_HEADER_VALUE, nextByte) != 0) {
			currState->isError = true;
			currState->errorState = HTTP_INNER_HEADER_VALUE_BUFFER_FULL;
			return -1;
		}
		break;
	case HEADER_RESPONSE_NEW_LINE_R:
		//convert
	case INNER_HEADER_FINISHED_PARSING:
	default:
		;
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
	const char *currPtr;
	for (i = 0; i < packetBufferLength; i++) {
		//DEBUG
		currPtr = packetBuffer + i;
		//-DEBUGE
		switch (nextState.currentActiveHeaderParser) {
		case HEADER_PARSER_STATUS_LINE:
			if (nextState.statusLineParserState.currentActiveStatusLineParser
					== HEADER_PARSER_STATUS_LINE_PARSER_REQUEST) {
				headerParserProcessRequestStatusLineByte(
						&(nextState.statusLineParserState.requestStatusLineState),
						packetBuffer[i]);
				//error handling
				nextState.isError 		= nextState.statusLineParserState.requestStatusLineState.isError;
				nextState.errorState 	= nextState.statusLineParserState.requestStatusLineState.errorState;

				//handle the end case
				if(nextState.statusLineParserState.requestStatusLineState.stateVal == HEADER_REQUEST_FINISHED_PARSING){
					nextState.currentActiveHeaderParser = HEADER_PARSER_INNER_HEADERS;
				}
			} else if (nextState.statusLineParserState.currentActiveStatusLineParser
					== HEADER_PARSER_STATUS_LINE_PARSER_RESPONSE) {
				headerParserProcessResponseStatusLineByte(
						&(nextState.statusLineParserState.responseStatusLineState),
						packetBuffer[i]);
				//error handling
				nextState.isError 		= nextState.statusLineParserState.responseStatusLineState.isError;
				nextState.errorState 	= nextState.statusLineParserState.responseStatusLineState.errorState;

				//handle the end case
				if(nextState.statusLineParserState.responseStatusLineState.stateVal == HEADER_RESPONSE_FINISHED_PARSING){
					nextState.currentActiveHeaderParser = HEADER_PARSER_INNER_HEADERS;
				}
			}
			break;
		case HEADER_PARSER_INNER_HEADERS:
			headerParserProcessInnerHeaderByte(
					&(nextState.innerHeadersParserState), packetBuffer[i]);
			//error handling
			nextState.isError 		= nextState.innerHeadersParserState.isError;
			nextState.errorState 	= nextState.innerHeadersParserState.errorState;

			//parse any headers
			if(true){
				//todo
			}
			break;
		default:
			//TODO error
			;
		}
		//TODO Handle call back functions
	}
	return nextState;
}

////TODO DOCUMENT ME!!!!!!!!!!!!
////---------------------------------------------------------------
//HeaderParserState anyHttpPacketHeaderParserParseBuffer(
//		const HeaderParserState state, const char *packetBuffer,
//		const int packetBufferLength, char **parserBufferEndPtr,
//		HttpParserCallbackFunction *callbackFunctions) {
////---------------------------------------------------------------
//
////Check if we actually need to do any of the below, maybe this isn't a bad a case!
//	HttpRequestOrResponseType packetType; //TODO CONST
//	HeaderParserState nextState = state;
//	int amountOfCurrentBufferParsed; //TODO CONST
//	{
//		//buffer the first few bytes, and then loop and call the parser
//		int i;
//		for (i = 0; i < packetBufferLength; i++) {
//			if (_isCapitalLetter(packetBuffer[i])) {
//				const int length = state.firstFewBytesOfHttpPacketBufferLength;
//				if (length + 1 <= MAX_FIRST_FEW_BYTES_LENGTH) {	//+1 because of the '\0'
//					nextState.firstFewBytesOfHttpPacketBuffer[length] =
//							packetBuffer[i];
//					nextState.firstFewBytesOfHttpPacketBufferLength++;
//				} else {
//					//TODO RETURN ERROR BUFFER FULL!!! too many bytes at the start !!!
//				}
//			} else if (packetBuffer[i] == '/') {
//				packetType = HTTP_PACKET_TYPE_RESPONSE;
//			} else if (packetBuffer[i] == ' ') {
//				packetType = HTTP_PACKET_TYPE_REQUEST;
//			} else {
//				//ERROR BAD CHARACTER
//			}
//		}
//		amountOfCurrentBufferParsed = i;
//	}
//
//	const int amountOfCurrentBufferLeftToParse = packetBufferLength
//			- amountOfCurrentBufferParsed;
//
//	//if (we exited the loop because we know the packet type and not because we hit the end of the buffer)
//	//then (call the parser on the buffer data and the rest of the buffer)
//	if (amountOfCurrentBufferParsed != packetBufferLength) {
//
//		if (packetType == HTTP_PACKET_TYPE_REQUEST) {
//			//parse the buffered bytes
//			nextState = httpHeaderParserProcessBuffer(nextState,
//					state.firstFewBytesOfHttpPacketBuffer,
//					state.firstFewBytesOfHttpPacketBufferLength,
//					parserBufferEndPtr, callbackFunctions,
//					HTTP_PACKET_TYPE_REQUEST);
//
//			//then parse the rest of the buffer
//			nextState = httpHeaderParserProcessBuffer(nextState,
//					packetBuffer + amountOfCurrentBufferParsed,
//					amountOfCurrentBufferLeftToParse, parserBufferEndPtr,
//					callbackFunctions, HTTP_PACKET_TYPE_REQUEST);
//
//		} else if (packetType == HTTP_PACKET_TYPE_RESPONSE) {
//			//parse the buffered bytes
//			nextState = httpHeaderParserProcessBuffer(nextState,
//					state.firstFewBytesOfHttpPacketBuffer,
//					state.firstFewBytesOfHttpPacketBufferLength,
//					parserBufferEndPtr, callbackFunctions,
//					HTTP_PACKET_TYPE_RESPONSE);
//
//			//then parse the rest of the buffer
//			nextState = httpHeaderParserProcessBuffer(nextState,
//					packetBuffer + amountOfCurrentBufferParsed,
//					amountOfCurrentBufferLeftToParse, parserBufferEndPtr,
//					callbackFunctions, HTTP_PACKET_TYPE_RESPONSE);
//		}
//	}
//
//	return nextState;
//}

