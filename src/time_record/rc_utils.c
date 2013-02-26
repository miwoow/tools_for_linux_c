#include "rc_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void
p_event(rc_event event)
{
//	printf("name: %s\ntag: %s\ns_time: %s\ne_time: %s\nid: %s\n", event.name, event.tags, ctime(&(event.s_time)), ctime(&(event.e_time)), event.id);
}

void
p_config(rc_config config)
{
//	printf("action_type : %d\neid : %s\npeid : %s\nfrom: %sto: %stype: %d\ntag: %s\nname: %s\n", config.action, config.eid, config.peid, ctime(&(config.from)), ctime(&(config.to)), config.type, config.tag, config.name);
}

int
rc_trans_time(time_t time, char *time_str)
{
	struct tm *ttime = gmtime(&time);
	snprintf(time_str, BUF_LEN, "%4d-%02d-%02d %02d:%02d:%02d", ttime->tm_year + 1900, ttime->tm_mon + 1, ttime->tm_mday, ttime->tm_hour + 8, ttime->tm_min, ttime->tm_sec);
	return EXIT_FAILURE;
}

void
free_event(rc_event *event)
{
	tag *tmp_tag;
	tmp_tag = event->tags;
	while(tmp_tag) {
		free(tmp_tag);
		tmp_tag = tmp_tag->next;
	}
}
