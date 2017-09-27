#ifndef FEUCHT_HOST
	#error "FEUCHT_HOST was not defined."
#endif

#ifndef FEUCHT_PORT
	#error "FEUCHT_PORT was not defined"
#endif

#ifndef FEUCHT_INTERVAL
	#define FEUCHT_INTERVAL 10
#endif

int init_protocol(ipv6_addr_t *remote);
int update_humidity(char *buf, size_t count);
