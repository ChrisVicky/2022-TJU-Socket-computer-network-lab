#include "parse.h"
// #define DEBUG
/**
 * @brief 
 *	deal with parsing messages
 * @param buffer	: msgs recieved
 * @param size		: total size of msg
 * @param socketFd	: socket Number (Not used)
 *
 * @return 
 * 	-->	Request : Success
 * 	-->	NULL	: Failed
 */

Request * parse(char *buffer, int size, int socketFd) {
	//Differant states in the state machine
	enum {
		STATE_START = 0, STATE_CR, STATE_CRLF, STATE_CRLFCR, STATE_CRLFCRLF
	};

	int i = 0, state;
	size_t offset = 0;
	char ch;
	char buf[8192];
	memset(buf, 0, 8192);

	state = STATE_START;
	while (state != STATE_CRLFCRLF) {
		char expected = 0;

		if (i == size)
			break;

		ch = buffer[i++];
		buf[offset++] = ch;

		switch (state) {
			case STATE_START:
			case STATE_CRLF:
				expected = '\r';
				break;
			case STATE_CR:
			case STATE_CRLFCR:
				expected = '\n';
				break;
			default:
				state = STATE_START;
				continue;
		}

		if (ch == expected)
			state++;
		else
			state = STATE_START;

	}

	//Valid End State
	if (state == STATE_CRLFCRLF) {
#ifdef DEBUG
		LOG("TRY TO PARSE\n");
		LOG("Parsing MSG\n%s\n" ,buf);
#endif
		Request *request = (Request *) malloc(sizeof(Request));
		request->header_count=0;
		//TODO You will need to handle resizing this in parser.y
		request->headers = (Request_header *) malloc(sizeof(Request_header)*1);
		set_parsing_options(buf, i, request);
		if (yyparse() == SUCCESS) {
			return request;
		}
		else{
			yyrestart(yyin); // 输入文件重置
			
		}
	}
	//TODO Handle Malformed Requests
	printf("Parsing Failed\n");
	return NULL;
}

