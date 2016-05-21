#all: googleTest.out
all: payloadTest.out

payloadTest.out: test/httpParserPayload_unitTest.cpp c/httpParserPayload.cpp
	g++ -Wno-write-strings -std=c++11 -g -O0 ./lib/libgtest_main.a ./lib/libgtest.a test/httpParserPayload_unitTest.cpp -lpthread -o payloadTest.out

googleTest.out: 
	g++ -Wno-write-strings -std=c++11 ./test/sample1.cc ./test/sample1_unittest.cc ./lib/libgtest.a ./lib/libgtest_main.a -lpthread -o ./test.out
