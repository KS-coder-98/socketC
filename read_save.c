#include "read_save.h"

int safe_read(int fd, void* buffer, size_t size)
{
	size_t bytes_read = 0;
	int lastRead;
	while (bytes_read<size)
	{
		if ( (lastRead = read(fd, (char*)buffer+bytes_read, size-bytes_read)) == -1 ){
			if ( errno != EINTR )
				return -1;
		}
		bytes_read+=lastRead;
	}
	return 0;
}

int safe_write(int fd, void* buffer, size_t size)
{
	sleep(2);
	size_t bytes_save = 0;
	int lastWrite;
	int i =0;
	while (bytes_save<size)
	{
		if ( (lastWrite = write(fd, (char*)buffer+bytes_save, size-bytes_save)) == -1 )
		{
			if ( errno != EINTR )
			{
				return -1;
			}
		}
		bytes_save+=lastWrite;
	}
	return 0;
}

int getTypeMsg(char* buffer)
{
	if ( buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] ==1 )
		return CALCULATE_SQRT_RYQ;
	if ( buffer[0] == 1 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] ==1 )
		return CALCULATE_SQRT_ANS;
	if ( buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] ==2 )
		return GET_TIME_REQ;
	if ( buffer[0] == 1 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] ==2 )
		return GET_TIME_ANS;
}

void setFlag(void* buffer, int typeMsg)
{
	struct msg *pBuffer= buffer; 
    if ( typeMsg == CALCULATE_SQRT_RYQ ){
		pBuffer->f1 = 0;
		pBuffer->f2 = 0;
		pBuffer->f3 = 0;
		pBuffer->f4 = 1;
    }else if ( typeMsg == CALCULATE_SQRT_ANS )
	{
		pBuffer->f1 = 1;
		pBuffer->f2 = 0;
		pBuffer->f3 = 0;
		pBuffer->f4 = 1;
	}else if ( typeMsg == GET_TIME_ANS )
	{
		pBuffer->f1 = 1;
		pBuffer->f2 = 0;
		pBuffer->f3 = 0;
		pBuffer->f4 = 2;
	}else if ( typeMsg == GET_TIME_REQ )
	{
		pBuffer->f1 = 0;
		pBuffer->f2 = 0;
		pBuffer->f3 = 0;
		pBuffer->f4 = 2;
	}
}

int generateReqIdInProcess()
{
	printf("generowanie id = %d\n", REQ_ID);
    return REQ_ID++;
}

void setReqIdInMsg(void* buffer, int reqId)
{
	struct msg *tempMsg = (struct msg*)buffer;
	tempMsg->rq_id = reqId;
}

void setSqrtValueInMsg(void * buffer, double value)
{
	// struct msg *tempMsg = (struct msg*)buffer;
	memcpy(buffer+8, &value, sizeof(double));
}

u_int32_t getReqIdFromMsg(void* buffer)
{
	struct msg *tempMsg = (struct msg*)buffer;
	return tempMsg->rq_id;
}


void procesMsg(void *msg)
{
	int typeMsgValue = getTypeMsg(msg);
	printf("REQ ID => %d\n", getReqIdFromMsg(msg));
	if ( typeMsgValue == CALCULATE_SQRT_RYQ ){
		printf("Type msg CALCULATE_SQRT_RYQ\n");
		procesSqrtMsgReq(msg);
	}else if ( typeMsgValue == CALCULATE_SQRT_ANS ){
		printf("CALCULATE_SQRT_ANS\n");
		procesSqrtMsgAns(msg);
	}else if ( typeMsgValue == GET_TIME_ANS ){
		printf("GET_TIME_ANS\n");
		procesTimeMsgAns(msg);
	}else if ( typeMsgValue == GET_TIME_REQ ){
		printf("GET_TIME_REQ\n");
		procesTimeMsgReq(msg);
	}else{
		printf("error -> wrong time msg\n");
	}
}

void procesTimeMsgAns(void *msg)
{
	struct msg *tempMsg = msg;
	size_t size_time;
	time_t time_from_server;
	memcpy(&size_time, tempMsg->data, sizeof(size_t));
	memcpy(&time_from_server, tempMsg->data+size_time, size_time);
	struct tm *timeHuman;
	timeHuman = localtime(&time_from_server);
	char tabTemp[30];
	strftime(tabTemp, 30, "%H:%M:%S", timeHuman);
	printf("seconds %s\n", tabTemp);
}

void procesTimeMsgReq(void *msg)
{
	setFlag(msg, GET_TIME_ANS);
	time_t now = time(NULL);
	printf("time set by server %ld\n", now);
	size_t size_time = sizeof(time_t);
	struct msg *tempMsg = msg;
	// void * pointerForTime = tempMsg->data;
	//set length time
	memcpy(tempMsg->data, &size_time, size_time);
	//set time in msg
	memcpy(tempMsg->data+size_time, &now, size_time);
}

void procesSqrtMsgAns(void *msg)
{
	struct msg *tempMasg = msg;
	double value;
	memcpy(&value, tempMasg->data, sizeof(double));
	if ( __FLOAT_WORD_ORDER__ != BIG_ENDIAN ){
		value = htobe64(value);
	}
	printf("square root is equl %lf\n", value);
}

void procesSqrtMsgReq(void *msg)
{
	struct msg *tempMasg = msg;
	setFlag(msg, CALCULATE_SQRT_ANS);
	setReqIdInMsg(msg, generateReqIdInProcess());
	double value;
	memcpy(&value, tempMasg->data, 8);
	if ( __FLOAT_WORD_ORDER__ != BIG_ENDIAN ){
		value = htobe64(value);
	}
	double tempSqrt = sqrt(value);
		if ( __FLOAT_WORD_ORDER__ != BIG_ENDIAN ){
		tempSqrt = htobe64(tempSqrt);
	}
	setSqrtValueInMsg(msg, tempSqrt);
}
