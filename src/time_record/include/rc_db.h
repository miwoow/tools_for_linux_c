#ifndef RC_DB_H
#define RC_DB_H
#include "rc_type.h"

#include <mongo.h>

void _conn();
void _dis_conn();
int rc_save_event(const rc_event *event);
void _eventobj_to_bson(const rc_event *event, bson *b);
void rc_query_event(rc_config *config);
void rc_update_event(rc_config *config);


#endif
