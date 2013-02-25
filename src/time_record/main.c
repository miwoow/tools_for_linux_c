#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rc_cmd.h"

#include <time.h>


int 
start_event(const rc_config config, rc_event *event)
{
	strncpy(event->name, config.name, BUF_LEN);
	time(&(event->s_time));
	strncpy(event->tag, config.tag, BUF_LEN);

	return EXIT_SUCCESS;
}

int
main(int argc, char **argv)
{
	rc_config config;
	rc_event event;
	memset(&config, 0, sizeof(rc_config));
	memset(&event, 0, sizeof(rc_event));
	rc_parse_cmd(argc, argv, &config);

	switch(config.action) {
		case 0:
			break;
		case 1:
			// start event.
			start_event(config, &event);
			p_event(event);
			break;
		case 2:
		break;
	}
	
	p_config(config);

	return 0;
}
