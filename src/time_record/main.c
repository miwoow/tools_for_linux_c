#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rc_cmd.h"
#include "rc_db.h"
#include "rc_utils.h"
#include "ks_log.h"
#include "rc_action.h"

int
main(int argc, char **argv)
{

	ks_log_init("/tmp/xd.log", KS_LOG_LEVEL_DEBUG);
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
			rc_start_event(&config, &event);
			p_event(event);
			break;
		case 2:
			// end event
			rc_end_event(&config);
			break;
		case 3:
			// list
			rc_list_event(&config);
			break;
		case 4:
			// start immediate event
			rc_starti_event(&config, &event);
			break;
		case 5:
			// add tag
			rc_add_tag(&config);
			break;
		case 6:
			// del tag
			break;
		case 7:
			// new child 
			rc_new_child(&config, &event);
			break;
		case 8:
			// end child
			rc_end_child(&config, &event);
			break;
		case 9:
			// del event
			rc_del_event(&config);
			break;
		case 10:
			rc_add_desc(&config);
			break;
		default:
			break;
	}
	free_event(&event);
	ks_log_exit();
	return 0;
}
