#ifndef FEUCHT_HOST
#error "FEUCHT_HOST was not defined."
#endif

#ifndef FEUCHT_PORT
#error "FEUCHT_PORT was not defined"
#endif

#ifndef FEUCHT_INTERVAL
#define FEUCHT_INTERVAL 60
#endif

/**
 * Initialize the protocol for a protocol server located at the given
 * remote address.
 *
 * @param remote Address of the protocol server.
 * @return Negative errno value on failure and zero on success.
 */
int init_protocol(ipv6_addr_t *remote);

/**
 * Free all resources associated with the protocol.
 */
void free_protocol(void);

/**
 * Transmit a new value to the protocol server.
 *
 * @param buf Pointer to a memory location containing the data which
 *	should be transmitted to the server.
 * @param count Amount of bytes that should be transmitted.
 * @return Negative errno value on failure and zero on success.
 */
int update_humidity(char *buf, size_t count);
