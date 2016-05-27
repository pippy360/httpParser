#include "../include/gtest/gtest.h"
#include "../c/httpParserPayload.cpp"

void testNonZeroChunk(const char *chunk, int noOfLengthChars, int sizeOfChunk) {

	int validTypes[] = { CHUNKED_PAYLOAD_LENGTH_CHAR, CHUNKED_LENGTH_BYTE,
			noOfLengthChars, CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R,
			CHUNKED_NEW_LINE_R, 1, CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N,
			CHUNKED_NEW_LINE_N, 1, CHUNKED_PAYLOAD_DATA, CHUNKED_DATA_BYTE,
			sizeOfChunk, CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R,
			CHUNKED_NEW_LINE_R, 1, CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N,
			CHUNKED_NEW_LINE_N, 1, };

	ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret;
	ChunkedPayloadParserStateEnum nextStateVal =
			CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N;
	const char *chunkPos = chunk;

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < validTypes[i * 3 + 2]; j++) {
			if (validTypes[i * 3] == CHUNKED_PAYLOAD_DATA) {
				ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal,
						*chunkPos, sizeOfChunk - j);
			} else {
				ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal,
						*chunkPos, 0);
			}
			nextStateVal = ret.chunkedStateEnum;

			EXPECT_EQ(validTypes[i * 3], ret.chunkedStateEnum);
			EXPECT_EQ(false, ret.isError);
			EXPECT_EQ(validTypes[i * 3 + 1], ret.byteType);

			chunkPos++;
		}
	}
}

TEST(getNextStateAndByteTypeForChunkedPacket_testing, test_zero_chunk) {
	const char *chunk = "0\r\n\r\n";
	int validTypes[] = { CHUNKED_PAYLOAD_LENGTH_CHAR, CHUNKED_LENGTH_BYTE, 1,
			CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R, CHUNKED_NEW_LINE_R, 1,
			CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, CHUNKED_NEW_LINE_N, 1,
			CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R, CHUNKED_NEW_LINE_R, 1,
			CHUNKED_PAYLOAD_PACKET_END, CHUNKED_NEW_LINE_N, 1, };

	ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret;
	ChunkedPayloadParserStateEnum nextStateVal =
			CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N;
	const char *chunkPos = chunk;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < validTypes[i * 3 + 2]; j++) {
			ret = getNextStateAndByteTypeForChunkedPacket(nextStateVal,
					*chunkPos, 0);
			nextStateVal = ret.chunkedStateEnum;

			EXPECT_EQ(validTypes[i * 3], ret.chunkedStateEnum);
			EXPECT_EQ(false, ret.isError);
			EXPECT_EQ(validTypes[i * 3 + 1], ret.byteType);

			chunkPos++;
		}
	}
}

void testNonZeroChunkWrapper(const char *chunk, const char *chunkSize) {
	testNonZeroChunk(chunk, strlen(chunkSize), strtol(chunkSize, nullptr, 16));
}

PayloadParserStateChunkedPayloadState testStructState = { PAYLOAD_START, "\0",
		0, 0, 0, false, NO_ERROR };

const char *testPkt =
		"4\r\nWiki\r\n5\r\npedia\r\nE\r\nin\r\n\r\nchunks. \r\n11\r\n12345678901234567\r\n0\r\n\r\n";
size_t testPktLen = std::strlen(testPkt);
//void getNextStateAndByteTypeForChunkedPacket_test_trivial_case_valid_packet(){
TEST(getNextStateAndByteTypeForChunkedPacket_testing, test_trivial_cases_with_valid_packet) {
	//test all the states manually

	testNonZeroChunkWrapper(testPkt, "4");
	testNonZeroChunkWrapper(testPkt + 9, "5");
	testNonZeroChunkWrapper(testPkt + 19, "E");
	testNonZeroChunkWrapper(testPkt + 38, "11");
	//testChunkWrapper(testPkt+61, "0");
}

void checkCommonErrorStateValues(
		ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret) {
	EXPECT_EQ(ERROR_PAYLOAD_PARSER, ret.chunkedStateEnum);
	EXPECT_EQ(true, ret.isError);
	EXPECT_EQ(CHUNKED_INVALID_CHAR, ret.byteType);
}

//getNextStateAndByteTypeForChunkedPacket_test_all_possible_errors
TEST(getNextStateAndByteTypeForChunkedPacket_testing, test_error_states) {
	PayloadParserStateChunkedPayloadState tempState = testStructState;
	ReturnValueOfGetNextStateAndByteTypeForChunkedPacket ret;
	ChunkedPayloadParserStateEnum nextStateVal =
			testStructState.chunkedStateEnum;

	ret = getNextStateAndByteTypeForChunkedPacket(PAYLOAD_START, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_LENGTH_BYTE, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_R, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_CR, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_CR, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_CR, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			CHUNKED_PAYLOAD_LENGTH_END_NEW_LINE_N, '\0', -1);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_DATA, '\0',
			0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_CR, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_DATA, '\0',
			-1);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(SERVER_ERROR_NEGATIVE_PAYLOAD_BYTES_REMAINING, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			CHUNKED_PAYLOAD_DATA_END_NEW_LINE_R, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_NEW_LINE, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			CHUNKED_PAYLOAD_DATA_END_NEW_LINE_N, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_LENGTH_BYTE, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			CHUNKED_PAYLOAD_PACKET_END_NEW_LINE_R, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_NEW_LINE, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_LENGTH_CHAR,
			'\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(EXPECTED_CR_OR_LENGTH_BYTE, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(CHUNKED_PAYLOAD_PACKET_END,
			'\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(PARSING_FINISHED_CANT_PROCESS_MORE_BYTES, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(ERROR_PAYLOAD_PARSER, '\0',
			0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(INPUT_STATE_WAS_ERROR_STATE, ret.errorState);

	ret = getNextStateAndByteTypeForChunkedPacket(
			(ChunkedPayloadParserStateEnum) 9999999, '\0', 0);
	checkCommonErrorStateValues(ret);
	EXPECT_EQ(BAD_STATE_VALUE, ret.errorState);
}

void getNextStateAndByteTypeForChunkedPacket_test_error_strings() {
	//test the returned error message
	//TODO
}

//TODO
void chunkedPayloadParserProcessByte_test_trivial_case() {
	PayloadParserStateChunkedPayloadState stateStruct;
	char *packet;

	packet = "3\r\nthi\r\ns is a packet";
	stateStruct = testStructState;
	chunkedPayloadParserProcessByte(&stateStruct, '\0');

	packet = "3\r\nthi\r\ns is a packet";
	stateStruct = testStructState;
	chunkedPayloadParserProcessByte(&stateStruct, '\0');

	packet = "3\r\nthi\r\ns is a packet";
	stateStruct = testStructState;
	chunkedPayloadParserProcessByte(&stateStruct, '\0');

	packet = "3\r\nthi\r\ns is a packet";
	stateStruct = testStructState;
	chunkedPayloadParserProcessByte(&stateStruct, '\0');

}

//trivial test
//void chunkedPayloadParserProcessBuffer_test_simple_test_trivial_case() {
TEST(chunkedPayloadParserProcessBuffer, test_simple_test_trivial_cases) {
	PayloadParserStateChunkedPayloadState stateStruct = testStructState;
	char *packet = "3\r\nthi\r\ns is a packet";
	//chunkedPayloadParserProcessBuffer(stateStruct, packet, strlen(packet), nullptr);
	//TODO
}

//test the buffer overflows tests too
//void chunkedPayloadParserProcessBuffer_test_simple_test_buffer_tests() {
TEST(chunkedPayloadParserProcessBuffer, simple_buffer_tests) {
	PayloadParserStateChunkedPayloadState stateStruct = testStructState;
	char *packet = "3\r\nthi\r\ns is a packet";
	//chunkedPayloadParserProcessBuffer(stateStruct, packet, strlen(packet), nullptr);
	//TODO
}

void chunkedPayloadParserProcessBuffer_test_error_cases() {
	//test the error states
	{
		//test 5\r\n\r\n !that should work !!
		//test ...someData\r\n\r\n //that should NOT work
		//test 0\r\ntest\r\n
	}
}

TEST(full, chunked_test) {

//	{
//		char *packet = "3\r\nthi\r\n0\r\n\r\n";
//		PayloadParserStateChunkedPayloadState state = { PAYLOAD_START,
//				"\0", 0, 0, 0, 0, 0, NO_ERROR };
//		state = httpPayloadParserProcessBuffer(state, packet, strlen(packet),
//		nullptr, nullptr);
//
//		if (state.isError) {
//			FAIL();
//		}
//	}
//
//	{
//		const char *packet = "11\r\n12345678901234567\r\n0\r\n\r\n";
//		PayloadParserState state = { PAYLOAD_START,
//				CHUNKED_ENCODING_HTTP_PACKET, 0, 0, 0, "\0", 0, 0, NO_ERROR };
//		state = httpPayloadParserProcessBuffer(state, packet, strlen(packet),
//		nullptr, nullptr);
//
//		if (state.isError) {
//			FAIL();
//		}
//	}
//
//	{
//		const char *packet =
//				"4\r\nWiki\r\n5\r\npedia\r\nE\r\nin\r\n\r\nchunks. \r\n11\r\n12345678901234567\r\n0\r\n\r\n";
//		PayloadParserState state = { PAYLOAD_START,
//				CHUNKED_ENCODING_HTTP_PACKET, 0, 0, 0, "\0", 0, 0, NO_ERROR };
//		state = httpPayloadParserProcessBuffer(state, packet, strlen(packet),
//		nullptr, nullptr);
//
//		if (state.isError) {
//			FAIL();
//		}
//	}
}

