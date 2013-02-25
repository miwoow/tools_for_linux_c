#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rc_cmd.h"

int
main(int argc, char **argv)
{
	rc_config config;
	memset(&config, 0, sizeof(rc_config));
	rc_parse_cmd(argc, argv, &config);

	static struct option long_options[] = {
		{"action", required_argument, 0, 0},
		{"tag", required_argument, 0, 0},
		{"name", required_argument, 0, 0},
		{0, 0, 0, 0}
	};

	switch(config.action) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
	}

	printf("action_type : %d\neid : %s\npeid : %s\nfrom: %sto: %stype: %d\ntag: %s\nname: %s\n", config.action, config.eid, config.peid, ctime(&(config.from)), ctime(&(config.to)), config.type, config.tag, config.name);

	return 0;
}
