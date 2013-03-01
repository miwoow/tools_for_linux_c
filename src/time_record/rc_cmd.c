#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <getopt.h>
#include "rc_cmd.h"

#define G_EVENT_STATUS_LEN 4

char *g_action_type[] = {"none", "new", "end", "list", "newi", "atag", "dtag", "newc", "endc"/*这个暂时没用，endc和end是一样的*/, "del", "adesc" };
char *g_event_status[] = {"running", "done", "pchild", "all"};

int
rc_parse_cmd(int argc, char **argv, rc_config *config)
{
	int c, i;
	tag *one_tag;
	int option_index;
	static struct option long_options[] = {
		{"action", required_argument, NULL, 'a'},
		{"tag", required_argument, NULL, 'g'},
		{"name", required_argument, NULL, 'n'},
		{"type", required_argument, NULL, 'b'},
		{"eid", required_argument, NULL, 'e'},
		{"peid", required_argument, NULL, 'p'},
		{"desc", required_argument, NULL, 'd'},
		{"day", required_argument, NULL, 'y'},
		{0, 0, 0, 0}
	};

	while(1) {
		c = getopt_long(argc, argv, "a:d:g:n:b:e:p:y:", long_options, &option_index);
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
			case 'd':
				strncpy(config->desc, optarg, LINE_LEN);
				break;
			case 'y':
			    // list which day's info
				config->day = atoi(optarg);
				break;
			case 'b':
				for (i=0; i< G_EVENT_STATUS_LEN; i++) {
					if (strcasecmp(optarg, g_event_status[i]) == 0) {
						config->status = i;
						break;
					}
				}
				break;
			case 'p':
				strncpy(config->peid, optarg, BUF_LEN);
				break;
			case 'e':
				strncpy(config->eid, optarg, BUF_LEN);
				break;
			case 'g':
				if (strlen(optarg) > BUF_LEN) {
					printf("tag name too long. < %d\n", BUF_LEN);
					break;
				}
				one_tag = (tag *)malloc(sizeof(tag));
				strncpy(one_tag->name, optarg, BUF_LEN);
				one_tag->next = config->tags;
				config->tags = one_tag;
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
