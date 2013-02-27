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
	bson_append_int(b, "status", event->status);
	bson_finish(b);
}

int 
rc_save_event(const rc_event *event)
{
	bson b;
	_conn();
	_eventobj_to_bson(event, &b);
	mongo_insert(&conn, ns, &b, NULL);
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

	_conn();

	bson_init(&query);
	bson_append_int(&query, "status", 0);
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
								printf("\n-->%s\n", bson_iterator_string( &it2 ));
								break;
							default:
								printf("\n--->other\n");
								break;
						}
					}
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
	}
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
