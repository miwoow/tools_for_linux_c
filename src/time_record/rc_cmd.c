#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <getopt.h>
#include "rc_cmd.h"




int
rc_parse_cmd(int argc, char **argv, rc_config *config)
{
	int c, i;
	int option_index;
	while(1) {
		c = getopt_long(argc, argv, "a:g:n:", long_options, &option_index);
		if (c == -1) {
			break;
		}
		switch(c) {
			case 'a':
				for (i=0; i<ACTION_TYPE_NUM; i++) {
					if (strcasecmp(optarg, g_action_type[i]) == 0) {
						config->action = i;
						break;
					}
				} 
				break;
			case 'g':
				strncpy(config->tag, optarg, BUF_LEN);
				break;
			case 'n':
				strncpy(config->name, optarg, BUF_LEN);
				break;

			default:
				break;
		}
	}
	return EXIT_SUCCESS;
}
