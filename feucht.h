#ifndef FEUCHT_HOST
	#error "FEUCHT_HOST was not defined."
#endif

#ifndef FEUCHT_PORT
#if FEUCHT_PROTO == __9P__
	#define FEUCHT_PORT 5640
#elif FEUCHT_PORT == __COAP__
	#define FEUCHT_PORT 5683
#else
	#error "Unknown FEUCHT_PROTO."
#endif
#endif

int init_protocol(void);
int update_humidity(char *buf, size_t count);
