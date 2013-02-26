#ifndef RC_TYPE_H
#define RC_TYPE_H

#include <time.h>

#define EVENT_TYPE_RUNNING 0
#define EVENT_TYPE_PARENT_CHILD 1
#define ACTION_TYPE_NUM 4
#define BUF_LEN 64

typedef enum _EVENT_STATUS { RUNNING, PARCHILD } EVENT_STATUS;

typedef struct _tag
{
	char name[BUF_LEN];
	struct _tag *next;
}tag;

typedef struct _rc_config
{
	int action;
	char eid[BUF_LEN];
	char peid[BUF_LEN];
	time_t from;
	time_t to;
	int type;
	tag *tags;
	char name[BUF_LEN];
}rc_config;

typedef struct _rc_event
{
	char name[BUF_LEN];
	tag *tags;
	time_t s_time;
	time_t e_time;
	char id[BUF_LEN];
	EVENT_STATUS status;
}rc_event;

#endif


