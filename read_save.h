#ifndef READ_SAVE_H
#define READ_SAVE_H
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define GET_TIME_REQ 1
#define CALCULATE_SQRT_RYQ 2
#define GET_TIME_ANS 3
#define CALCULATE_SQRT_ANS 4

#define MAX_SIZE_DATA_MSG 64 // I think this is enought to data

// struct msg
// {
// 	char f1;
// 	char f2;
// 	char f3;
// 	char f4;
// 	u_int32_t rq_id;
// 	double value;
// };

struct msg
{
	char f1;
	char f2;
	char f3;
	char f4;
	u_int32_t rq_id;
    char data[MAX_SIZE_DATA_MSG];
};

#define SIZE_BUFFER sizeof(struct msg)

//comunications
int safe_read(int fd, void* buffer, size_t size);
int safe_write(int fd, void* buffer, size_t size);

//getters
int getTypeMsg(char* buffer);
u_int32_t getReqIdFromMsg(void* buffer);

//setters
void setFlag(void* buffer, int typeMsg);
void setReqIdInMsg(void* buffer, int reqId);
void setSqrtValueInMsg(void * buffer, double value);

static int REQ_ID = 1;
int generateReqIdInProcess();

//precesing
void procesMsg(void *msg);
void procesTimeMsgAns(void *msg);
void procesTimeMsgReq(void *msg);
void procesSqrtMsgAns(void *msg);
void procesSqrtMsgReq(void *msg);

#endif READ_SAVE_H