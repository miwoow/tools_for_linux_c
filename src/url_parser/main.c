#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "apr_general.h"
#include "apr_strings.h"
#include "apr_uri.h"
#include "apr_pools.h"

apr_pool_t *p;

void usage()
{
	printf("Usage: ./url_parser http://www.baidu.com\n");
}

int main(int argc, char **argv)
{
	if (argc < 2) 
	{
		usage();
		exit(1);
	}

	apr_uri_t info;
	apr_initialize();

	apr_pool_create(&p, NULL);
	apr_uri_parse(p, argv[1], &info);

	printf("scheme: %s\n", info.scheme);
	printf("hostname: %s\n", info.hostname);
	printf("hostinfo: %s\n", info.hostinfo);
	printf("port: %s\n", info.port_str);
	printf("path: %s\n", info.path);
	printf("query: %s\n", info.query);

	apr_pool_destroy(p);

	return 0;
}
