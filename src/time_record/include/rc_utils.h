#ifndef RC_UTILS_H
#define RC_UTILS_H

#include "rc_type.h"

void p_event(rc_event event);
void p_config(rc_config config);
int rc_trans_time(time_t time, char *time_str);
void free_event(rc_event *event);

#endif
