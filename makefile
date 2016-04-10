all: payloadTest.out

payloadTest.out: test/httpParserPayload_unitTest.cpp c/httpParserPayload.cpp
	g++ -std=c++11 -g -O0 test/httpParserPayload_unitTest.cpp -o payloadTest.out 


