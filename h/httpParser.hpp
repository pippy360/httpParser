typedef enum {
	//first byte of the input buffer is in...
	HTTP_PACKET_STATUS_LINE,//...in the status line
	HTTP_PACKET_HEADERS,//...in the header
	HTTP_PACKET_PAYLOAD,//...in the payload
	HTTP_PACKET_FINISHED
} ParserOverviewStateEnum;

typedef struct {
	ParserOverviewStateEnum stateEnum;
	long bytesProcessed;//
} ParserOverviewState;
