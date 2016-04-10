#include "../c/httpParserPayload.cpp"


//TODO make sure to test 0\r\ndata....

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

void checkCommonErrorStateValues(ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret){
	if (ret.payloadParserStateEnum != ERROR_PAYLOAD_PARSER){
		//fail()
		printf("we got an error :( 2\n");
	}
	if (ret.isError != true){
		//fail()
		printf("we got an error :( _\n");
	}
	if (ret.byteType != INVALID_CHAR){
		//fail()
		printf("we got an error :( __\n");
	}
}

void getNextStateAndByteTypeForChunkedPacket_test_all_possible_errors(){
	//TODO:
	PayLoadParserState tempState = testStructState;
	ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret;
	PayloadParserStateEnum nextStateVal = testStructState.stateVal;

	ret = getNextStateAndByteTypeForChunkedPacket(PAYLOAD_START, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_LENGTH_BYTE){
		//fail()
		printf("we got an error :( 1\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_CR){
		//fail()
		printf("we got an error :( 2\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, '\0', 0, true);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_CR){
		//fail()
		printf("we got an error :( 3\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_CR){
		//fail()
		printf("we got an error :( 4\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, '\0', -1, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING){
		//fail()
		printf("we got an error :( 5\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_DATA, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_CR){
		//fail()
		printf("we got an error :( 6\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_DATA, '\0', -1, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING){
		//fail()
		printf("we got an error :( 7\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_NEW_LINE){
		//fail()
		printf("we got an error :( 8\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_CR){
		//fail()
		printf("we got an error :( 9\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_NEW_LINE){
		//fail()
		printf("we got an error :( 10\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_LENGTH_CHAR, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != EXPECTED_CR_OR_LENGTH_BYTE){
		//fail()
		printf("we got an error :( 11\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_PACKET_END, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != PARSING_FINISHED_CANT_PROCESS_MORE_BYTES){
		//fail()
		printf("we got an error :( 12\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket(ERROR_PAYLOAD_PARSER, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != INPUT_STATE_WAS_ERROR_STATE){
		//fail()
		printf("we got an error :( 13\n");
	}

	ret = getNextStateAndByteTypeForChunkedPacket((PayloadParserStateEnum) 9999999, '\0', 0, false);
	checkCommonErrorStateValues(ret);
	if(ret.errorState != BAD_STATE_VALUE){
		//fail()
		printf("we got an error :( 14\n");
	}
}

void getNextStateAndByteTypeForChunkedPacket_test_error_strings(){
}

void chunkedPayloadParserProcessBuffer_test_simple() {
	PayLoadParserState stateStruct = testStructState;
	char *packet = "3\r\nthi\r\ns is a packet";
	chunkedPayloadParserProcessBuffer(stateStruct, packet, strlen(packet), nullptr);

	//trivial test

	//test the buffer overflows tests too

	//test
}

void chunkedPayloadParserProcessBuffer_test_error_cases() {
	//test the error states
	{
		//test 5\r\n\r\n !that should work !!
		//test ...someData\r\n\r\n //that should NOT work
		//test 0\r\ntest\r\n
	}
}

int main(){

	getNextStateAndByteTypeForChunkedPacket_test_trivial_case_valid_packet();
	chunkedPayloadParserProcessBuffer_test_simple();
	getNextStateAndByteTypeForChunkedPacket_test_all_possible_errors();
	printf("done!\n");
	return 0;
}
