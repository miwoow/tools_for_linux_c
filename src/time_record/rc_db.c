#include "rc_db.h"


static char *mongo_host="127.0.0.1";
static unsigned int mongo_port=27017;
static mongo conn;
static mongo_write_concern concern;
static char *db_name = "rc_life";
static char *cllection_name = "rc_event";

void
_conn() {
	if (mongo_is_connected(&conn)) {
		return;
	} else {
		mongo_client(&conn, mongo_host, mongo_port);
		mongo_write_concern_init(&concern);
		concern.w = 1;
		mongo_write_concern_finish(&concern);
		mongo_set_write_concern(&conn, &concern);
	}
}

void 
_eventobj_to_bson(const rc_event *event, bson *b)
{
	char cindex[8];
	tag *tmp_tag;
	int i=0;
	bson_init(b);
	bson_append_new_oid(b, "_id");
	bson_append_string(b, "name", event->name);
	bson_append_start_array(b, "tags");
		tmp_tag = event->tags;
		while(tmp_tag) {
			snprintf(cindex, 8, "%d", i);
			bson_append_string(b, cindex, tmp_tag->name);
			tmp_tag = tmp_tag->next;
			i++;
		}
	bson_append_finish_array(b);
	bson_append_time_t(b, "s_time", event->s_time);
	bson_append_time_t(b, "e_time", event->e_time);
	bson_append_int(b, "status", event->status);
	bson_finish(b);
}

int 
rc_save_event(const rc_event *event)
{
	bson b;
	_conn();
	_eventobj_to_bson(event, &b);
	mongo_insert(&conn, "rc_life.rc_event", &b, NULL);
	return 0;
}




void _dis_conn()
{
	if (mongo_is_connected(&conn)) {
		mongo_write_concern_destroy(&concern);
		mongo_destroy(&conn);
	}
}
