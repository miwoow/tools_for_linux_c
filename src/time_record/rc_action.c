#include "rc_action.h"
#include <time.h>
#include <string.h>
#include "rc_db.h"

int
rc_gen_event(rc_config *config, rc_event *event)
{
	strncpy(event->name, config->name, BUF_LEN);
	time(&(event->s_time));
	event->tags = config->tags;
	config->tags = NULL;

	event->status = RUNNING;
	return EXIT_SUCCESS;
}

int
rc_starti_event(rc_config *config, rc_event *event)
{
	rc_gen_event(config, event);
	time(&(event->e_time));
	event->status = DONE;
	rc_save_event(event);
	return EXIT_SUCCESS;
}

int 
rc_start_event(rc_config *config, rc_event *event)
{
	rc_gen_event(config, event);
	rc_save_event(event);
	return EXIT_SUCCESS;
}

int 
rc_list_event(rc_config *config)
{
	rc_query_event(config);

	return EXIT_SUCCESS;
}

int rc_end_event(rc_config *config)
{
	rc_update_event(config);
	return EXIT_SUCCESS;
}

int rc_add_tag(rc_config *config)
{
	rc_mod_add_tag(config);
	return EXIT_SUCCESS;
}
