#ifndef RC_TYPE_H
#define RC_TYPE_H

#include <time.h>

#define EVENT_TYPE_RUNNING 0
#define EVENT_TYPE_PARENT_CHILD 1
#define ACTION_TYPE_NUM 3
#define BUF_LEN 64


typedef struct _rc_config
{
	int action;
	char eid[BUF_LEN];
	char peid[BUF_LEN];
	time_t from;
	time_t to;
	int type;
	char tag[BUF_LEN];
	char name[BUF_LEN];
}rc_config;

#endif


