#include "../include/gtest/gtest.h"
#include "../c/httpParserHeader.cpp"


TEST(getNextRequestStatusLineStateAndByteType, simple_test_case){

	{
		const char *requestStatusLine = "GET / HTTP/1.1\r\n";
		const char *currPos = requestStatusLine;

		int validTypes[] = {
			HEADER_REQUEST_METHOD, 				3,
			HEADER_REQUEST_SPACE_AFTER_METHOD, 	1,
			HEADER_REQUEST_PATH, 				1,
			HEADER_SPACE_AFTER_PATH,			1,
			HEADER_REQUEST_HTTP_VERSION_H, 		1,
			HEADER_REQUEST_HTTP_VERSION_T,		1,
			HEADER_REQUEST_HTTP_VERSION_T_2,	1,
			HEADER_REQUEST_HTTP_VERSION_P,		1,
			HEADER_REQUEST_HTTP_VERSION_SLASH,	1,
			HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER,	1,
			HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER,	1,
			HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER,	1,
			HEADER_REQUEST_NEW_LINE_R,			1,
			HEADER_REQUEST_FINISHED_PARSING,	1,
		};

		ReturnValueOfGetNextRequestStatusLineStateAndByteType ret;
		StatusLineRequestHeaderParserStateEnum nextStateVal = HEADER_REQUEST_START;

		for(int i = 0; i < 13; i++){
			for(int j=0; j < validTypes[i*2 + 1]; j++){
				ret = getNextRequestStatusLineStateAndByteType(nextStateVal, *currPos);
				nextStateVal = ret.statusLineParserStateEnum;

				EXPECT_EQ(validTypes[i*2]	, ret.statusLineParserStateEnum);
				EXPECT_EQ(false				, ret.isError);

				currPos++;
			}
		}
	}

	{
		const char *requestStatusLine = "GET /this&/is/a.path HTTP/1.1\r\n";
		const char *currPos = requestStatusLine;

		int validTypes[] = {
			HEADER_REQUEST_METHOD, 				3,
			HEADER_REQUEST_SPACE_AFTER_METHOD, 	1,
			HEADER_REQUEST_PATH, 				strlen("/this&/is/a.path"),
			HEADER_SPACE_AFTER_PATH,			1,
			HEADER_REQUEST_HTTP_VERSION_H, 		1,
			HEADER_REQUEST_HTTP_VERSION_T,		1,
			HEADER_REQUEST_HTTP_VERSION_T_2,	1,
			HEADER_REQUEST_HTTP_VERSION_P,		1,
			HEADER_REQUEST_HTTP_VERSION_SLASH,	1,
			HEADER_REQUEST_HTTP_VERSION_FIRST_NUMBER,	1,
			HEADER_REQUEST_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER,	1,
			HEADER_REQUEST_HTTP_VERSION_SECOND_NUMBER,	1,
			HEADER_REQUEST_NEW_LINE_R,			1,
			HEADER_REQUEST_FINISHED_PARSING,	1,
		};

		ReturnValueOfGetNextRequestStatusLineStateAndByteType ret;
		StatusLineRequestHeaderParserStateEnum nextStateVal = HEADER_REQUEST_START;

		for(int i = 0; i < 13; i++){
			for(int j=0; j < validTypes[i*2 + 1]; j++){
				ret = getNextRequestStatusLineStateAndByteType(nextStateVal, *currPos);
				nextStateVal = ret.statusLineParserStateEnum;

				EXPECT_EQ(validTypes[i*2]	, ret.statusLineParserStateEnum);
				EXPECT_EQ(false				, ret.isError);

				currPos++;
			}
		}
	}

}


TEST(getNextResponseStatusLineStateAndByteType, simple_test_case){

	{
		const char *responseStatusLine = "HTTP/1.1 200 OK\r\n";
		const char *currPos = responseStatusLine;

		int validTypes[] = {
			HEADER_RESPONSE_HTTP_VERSION_H,		1,
			HEADER_RESPONSE_HTTP_VERSION_T,		1,
			HEADER_RESPONSE_HTTP_VERSION_T_2,	1,
			HEADER_RESPONSE_HTTP_VERSION_P,		1,
			HEADER_RESPONSE_HTTP_VERSION_SLASH,	1,
			HEADER_RESPONSE_HTTP_VERSION_FIRST_NUMBER,				1,
			HEADER_RESPONSE_HTTP_VERSION_DOT_AFTER_FIRST_NUMBER,	1,
			HEADER_RESPONSE_HTTP_VERSION_SECOND_NUMBER,				1,
			HEADER_RESPONSE_SPACE_AFTER_VERSION,		1,
			HEADER_RESPONSE_STATUS_CODE,				3,
			HEADER_RESPONSE_SPACE_AFTER_STATUS_CODE,	1,
			HEADER_RESPONSE_REASON_PHRASE,				2,
			HEADER_RESPONSE_NEW_LINE_R,					1,
			HEADER_RESPONSE_FINISHED_PARSING,			1
		};

		ReturnValueOfGetNextResponseStatusLineStateAndByteType ret;
		StatusLineResponseHeaderParserStateEnum nextStateVal = HEADER_RESPONSE_START;

		for(int i = 0; i < 14; i++){
			for(int j=0; j < validTypes[i*2 + 1]; j++){
				ret = getNextResponseStatusLineStateAndByteType(nextStateVal, *currPos);
				nextStateVal = ret.statusLineParserStateEnum;

				EXPECT_EQ(validTypes[i*2]	, ret.statusLineParserStateEnum);
				EXPECT_EQ(false				, ret.isError);

				currPos++;
			}
		}
	}
}


TEST(getNextInnerHeaderStateAndByteType, simple_test_case){

	{
		const char *responseStatusLine = "tomwashere: doing this\r\n\r\n";
		const char *currPos = responseStatusLine;

		int validTypes[] = {
			INNER_HEADER_NAME,				10,
			INNER_HEADER_COLON,				1,
			INNER_HEADER_VALUE,				11,
			INNER_HEADER_NEW_LINE_R,		1,
			INNER_HEADER_NEW_LINE_N,		1,
			INNER_HEADER_FINAL_NEW_LINE_R,	1,
			INNER_HEADER_FINISHED_PARSING,	1
		};

		ReturnValueOfGetNextInnerHeaderStateAndByteType ret;
		InnerHeadersParserStateEnum nextStateVal = INNER_HEADER_START;

		for(int i = 0; i < 7; i++){
			for(int j=0; j < validTypes[i*2 + 1]; j++){
				ret = getNextInnerHeaderStateAndByteType(nextStateVal, *currPos);
				nextStateVal = ret.innerHeadersParserStateEnum;

				EXPECT_EQ(validTypes[i*2]	, ret.innerHeadersParserStateEnum);
				EXPECT_EQ(false				, ret.isError);

				currPos++;
			}
		}
	}

	{
		const char *responseStatusLine = "tomwashere: doing this\r\nanother: one\r\n\r\n";
		const char *currPos = responseStatusLine;

		int validTypes[] = {
			INNER_HEADER_NAME,				10,
			INNER_HEADER_COLON,				1,
			INNER_HEADER_VALUE,				11,
			INNER_HEADER_NEW_LINE_R,		1,
			INNER_HEADER_NEW_LINE_N,		1,
			INNER_HEADER_NAME,				7,
			INNER_HEADER_COLON,				1,
			INNER_HEADER_VALUE,				4,
			INNER_HEADER_NEW_LINE_R,		1,
			INNER_HEADER_NEW_LINE_N,		1,
			INNER_HEADER_FINAL_NEW_LINE_R,	1,
			INNER_HEADER_FINISHED_PARSING,	1
		};

		ReturnValueOfGetNextInnerHeaderStateAndByteType ret;
		InnerHeadersParserStateEnum nextStateVal = INNER_HEADER_START;

		for(int i = 0; i < 11; i++){
			for(int j=0; j < validTypes[i*2 + 1]; j++){
				ret = getNextInnerHeaderStateAndByteType(nextStateVal, *currPos);
				nextStateVal = ret.innerHeadersParserStateEnum;

				EXPECT_EQ(validTypes[i*2]	, ret.innerHeadersParserStateEnum);
				EXPECT_EQ(false				, ret.isError);

				currPos++;
			}
		}
	}
}


TEST(testing, aFullBuffer){
	HeaderParserState state = headerParserState_request_init;
	const char *packet = "GET / HTTP/1.1\r\ntom: somethingh\r\nfuckingcontentlength";
	state = httpHeaderParserProcessBuffer(state, packet, strlen(packet), nullptr, nullptr, HTTP_PACKET_TYPE_REQUEST);
	if(state.isError == true){
		FAIL();
	}
}



