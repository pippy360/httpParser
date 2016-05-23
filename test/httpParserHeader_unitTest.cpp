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
		const char *requestStatusLine = "GET /this/is/a.path HTTP/1.1\r\n";
		const char *currPos = requestStatusLine;

		int validTypes[] = {
			HEADER_REQUEST_METHOD, 				3,
			HEADER_REQUEST_SPACE_AFTER_METHOD, 	1,
			HEADER_REQUEST_PATH, 				strlen("/this/is/a.path"),
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
