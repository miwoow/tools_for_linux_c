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

	_conn();

	bson_init(&query);
	if (config->status != ALL) {
		bson_append_int(&query, "status", config->status);
	}
	bson_finish(&query);

	mongo_cursor_init(&cursor, &conn, ns);
	mongo_cursor_set_query( &cursor, &query);
	while(mongo_cursor_next(&cursor) == MONGO_OK) {
		bson_iterator it, it2;
		bson_iterator_init( &it, mongo_cursor_bson( &cursor ));
		while (bson_iterator_next(&it)) {
			printf("%s : ", bson_iterator_key( &it ));
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
					tt = bson_iterator_date( &it );
					rc_trans_time(tt/1000, time_str);
					printf("%s\n", time_str);
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

int rc_get_event(const char *eid)
{
	bson query;

	bson_init(&query);
	bson_finish(&query);

	bson_destroy(&query);
	return EXIT_SUCCESS;
}
