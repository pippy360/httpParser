#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

//returns 0 if success, non-0 otherwise
int _addToHttpStateBufferWithErrorChecking(char *buffer,
		int *currentLengthOfBuffer, const int maxSize, const char nextByte) {
	if (*currentLengthOfBuffer + 2 < maxSize) //+2 for the new char and '\0'
			{
		//strcpy_s(currState->chunkSizeStrBuffer, &nextByte);
		strcat(buffer, &nextByte);
		(*currentLengthOfBuffer)++;
		return 0;
	} else {
		return -1;
	}
}
