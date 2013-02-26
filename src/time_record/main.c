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
			free_event(&event);
			break;
		case 2:
			break;
		case 3:
			// list
			rc_list_event(&config);
			break;
		default:
			break;
	}
	ks_log_exit();
	return 0;
}
