#ifndef RC_ACTION_H
#define RC_ACTION_H

#include "rc_type.h"

int rc_start_event(rc_config *config, rc_event *event);
int rc_list_event(rc_config *config);
int rc_end_event(rc_config *config);
int rc_gen_event(rc_config *config, rc_event *event);
int rc_starti_event(rc_config *config, rc_event *event);
int rc_add_tag(rc_config *config);
int rc_new_child(rc_config *config, rc_event *event);
int rc_end_child(rc_config *config, rc_event *event);
int rc_del_event(rc_config *config);
int rc_add_desc(rc_config *config);

#endif
