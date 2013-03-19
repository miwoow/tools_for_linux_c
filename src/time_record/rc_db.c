#include "rc_db.h"
#include "rc_utils.h"
#include <time.h>


static char *mongo_host="127.0.0.1";
static unsigned int mongo_port=27017;
static mongo conn;
static mongo_write_concern concern;
static char *ns = "rc_life.rc_event";

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
	bson_append_string(b, "peid", event->peid);
	bson_append_int(b, "status", event->status);
	bson_finish(b);
}

int 
rc_save_event(const rc_event *event)
{
	bson b;
	_conn();
	_eventobj_to_bson(event, &b);
	if (mongo_insert(&conn, ns, &b, NULL) == MONGO_ERROR) {
		printf("error: %s\n", conn.errstr);
	}
	bson_destroy(&b);
	return 0;
}


void
rc_query_event(rc_config *config)
{
	bson query, sub;
	mongo_cursor cursor;
	char hex_oid[25];
	char time_str[BUF_LEN] = {0};
	time_t tt;
	int event_count=0;
	const char *key;
	struct tm *euse_time, *t_gm_time;

	_conn();

	bson_init(&query);
	if (config->status != ALL) {
		bson_append_int(&query, "status", config->status);
	}
	bson_append_start_object( &query, "s_time" );
	time(&tt);
	t_gm_time = gmtime(&tt);
	t_gm_time->tm_mday += config->day;
	t_gm_time->tm_hour = 0;
	t_gm_time->tm_min = 0;
	t_gm_time->tm_sec = 0;
	tt = mktime(t_gm_time);
	bson_append_time_t( &query, "$gte", tt );
	bson_append_finish_object( &query );
	
	bson_finish(&query);

	mongo_cursor_init(&cursor, &conn, ns);
	mongo_cursor_set_query( &cursor, &query);
	while(mongo_cursor_next(&cursor) == MONGO_OK) {
		bson_iterator it, it2;
		bson_iterator_init( &it, mongo_cursor_bson( &cursor ));
		while (bson_iterator_next(&it)) {
			key = bson_iterator_key( &it);
			printf("%s : ", key);
			switch( bson_iterator_type( &it )) {
				case BSON_INT:
					printf("%d\n", bson_iterator_int( &it ));
					break;
				case BSON_STRING:
					printf("%s\n", bson_iterator_string( &it ));
					break;
				case BSON_OID:
					bson_oid_to_string(bson_iterator_oid( &it ), hex_oid);
					printf("%s\n", hex_oid);
					break;
				case BSON_ARRAY:
					bson_iterator_subobject(&it, &sub);
					bson_iterator_init( &it2, &sub);
					while (bson_iterator_next( &it2 )) {
						switch (bson_iterator_type( &it2 )) {
							case BSON_STRING:
								printf("%s, ", bson_iterator_string( &it2 ));
								break;
							default:
								printf("\nError: other tag type.\n");
								break;
						}
					}
					printf("\n");
					break;
				case BSON_OBJECT:
					printf("bson object\n");
					break;
				case BSON_TIMESTAMP:
					printf("bson timestamp.\n");
					break;
				case BSON_DATE:
					tt = bson_iterator_date( &it )/ 1000;
					if (strcmp(key, "use_time") == 0) {
						euse_time = gmtime(&tt);
						p_time_spin(euse_time);
					} else {
						rc_trans_time(tt, time_str);
						printf("%s\n", time_str);
					}
					break;
				default:
					printf("other type\n");
			}
		}
		printf("======================================================\n");
		event_count++;
	}
	printf("Count Num: %d\n", event_count);
	bson_destroy( &query );
	mongo_cursor_destroy( &cursor );
}

void _dis_conn()
{
	if (mongo_is_connected(&conn)) {
		mongo_write_concern_destroy(&concern);
		mongo_destroy(&conn);
	}
}

void rc_update_event(rc_config *config)
{
	// end event
	bson cond, op;
	bson_oid_t oid;
	time_t end_time;
	rc_event t_event;

	_conn();

	bson_oid_from_string(&oid, config->eid);

	bson_init(&cond);
	bson_append_oid(&cond, "_id", &oid);
	//bson_append_int(&cond, "status", 0);
	// TODO: if there is no such event. print info.
	bson_finish(&cond);
	
	bson_init(&op);
	{
		bson_append_start_object( &op, "$set");
		bson_append_int( &op, "status", 1);
		time(&end_time);
		bson_append_time_t(&op , "e_time", end_time);
		rc_get_event(config->eid, &t_event);
		bson_append_time_t(&op, "use_time", end_time - t_event.s_time);
		bson_append_finish_object( &op );
	}
	bson_finish(&op);
	mongo_clear_errors(&conn);
	
	if (mongo_update(&conn, ns, &cond, &op, 0, NULL) == MONGO_ERROR) {
		printf("error:%d: %s\n", conn.errcode, conn.errstr);
	}

	bson_destroy(&cond);
	bson_destroy(&op);
}

int
rc_mod_add_tag(rc_config *config)
{
	bson cond, op;
	bson_oid_t oid;
	time_t end_time;
	tag *tmp_tag;
	int i;
	char index[8]={0};

	_conn();

	bson_oid_from_string(&oid, config->eid);

	bson_init(&cond);
	bson_append_oid(&cond, "_id", &oid);
	bson_finish(&cond);
	
	bson_init(&op);
	{
		bson_append_start_object( &op, "$pushAll");
		bson_append_start_array( &op, "tags");
		tmp_tag = config->tags;
		i=0;
		while(tmp_tag) {
			snprintf(index, 8, "%d", i++);
			bson_append_string(&op, index, tmp_tag->name);
			tmp_tag = tmp_tag->next;
		}
		bson_append_finish_array(&op);
		bson_append_finish_object( &op );
	}
	bson_finish(&op);
	mongo_clear_errors(&conn);
	
	if (mongo_update(&conn, ns, &cond, &op, 0, NULL) == MONGO_ERROR) {
		printf("error:%d: %s\n", conn.errcode, conn.errstr);
	}

	bson_destroy(&cond);
	bson_destroy(&op);
	return EXIT_SUCCESS;
}

int
rc_mod_del_event(rc_config *config)
{
	bson cond;
	bson_oid_t moid;

	_conn();

	bson_oid_from_string(&moid, config->eid);

	bson_init(&cond);
	bson_append_oid(&cond, "_id", &moid);
	bson_finish(&cond);
	
	if (mongo_remove(&conn, ns, &cond, NULL) == MONGO_ERROR) {
		printf("ERROR: %s\n", conn.errstr);
	}

	bson_destroy(&cond);
	return EXIT_SUCCESS;
}

int rc_mod_add_desc(rc_config *config)
{
	bson cond, op;
	bson_oid_t oid;
	time_t end_time;
	int i;
	char index[8]={0};

	_conn();

	bson_oid_from_string(&oid, config->eid);

	bson_init(&cond);
	bson_append_oid(&cond, "_id", &oid);
	bson_finish(&cond);
	
	bson_init(&op);
	{
		bson_append_start_object( &op, "$addToSet");
		bson_append_string(&op, "desc", config->desc);
		bson_append_finish_object( &op );
	}
	bson_finish(&op);
	mongo_clear_errors(&conn);
	
	if (mongo_update(&conn, ns, &cond, &op, 0, NULL) == MONGO_ERROR) {
		printf("error:%d: %s\n", conn.errcode, conn.errstr);
	}

	bson_destroy(&cond);
	bson_destroy(&op);
	return EXIT_SUCCESS;
}

int rc_iterator_to_event(bson_iterator *it, rc_event *e)
{
	const char *name = NULL; 
	while (bson_iterator_next( it )) {
		name = bson_iterator_key( it );
		if (strcmp(name, "name") == 0) {
			strncpy(e->name, bson_iterator_string(it), BUF_LEN);	
		} else if (strcmp(name, "s_time") == 0) {
			e->s_time = bson_iterator_date(it) / 1000;
		}
	}
	return EXIT_SUCCESS;
}

int rc_get_event(const char *eid, rc_event *e)
{
	bson query;
	bson obj;
	bson_oid_t moid;

	bson_oid_from_string(&moid, eid);

	bson_init(&query);
	bson_append_oid(&query, "_id", &moid);
	bson_finish(&query);

	if (mongo_find_one(&conn, ns, &query, 0, &obj) != MONGO_OK) {
		printf("can't find this obj.\n");
		return 1;
	} else {
		bson_iterator it;
		bson_iterator_init(&it, &obj);
		rc_iterator_to_event(&it, e);
	}

	bson_destroy(&query);
	bson_destroy(&obj);
	return EXIT_SUCCESS;
}
