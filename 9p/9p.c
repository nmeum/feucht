#include "net/af.h"
#include "net/netopt.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/tcp.h"

#include "9p.h"
#include "feucht.h"
#include "kernel_types.h"

#define GNRC_TIMEOUT 10 * 10000000

extern kernel_pid_t netif;

/**
 * GNRC transmission control block.
 */
static gnrc_tcp_tcb_t tcb;

/**
 * 9P connection context.
 */
static _9pctx ctx;

/**
 * Fid for the file containing the humidity values.
 */
static _9pfid *hfid;

/**
 * File name of the file containing the humidity values.
 */
#define HUMIDITY_FN "humidity"

/**
 * Function for reading from the GNRC tcp socket.
 *
 * @param buf Pointer to a buffer where received data should be stored.
 * @param count Maximum amount of bytes which should be read.
 * @return Amount of bytes received.
 */
static ssize_t
recvfn(void *buf, size_t count)
{
	int ret;
	size_t nbytes;
	netopt_state_t state;

	nbytes = gnrc_tcp_recv(&tcb, buf, count, GNRC_TIMEOUT);
	if (nbytes >= 0) {
		state = NETOPT_STATE_SLEEP;
		if ((ret = gnrc_netapi_set(netif, NETOPT_STATE,
				0, &state, sizeof(netopt_state_t)) < 0))
			return ret;
	}

	return nbytes;
}

/**
 * Function for writting to the GNRC tcp socket.
 *
 * @param buf Pointer to a buffer containing the data which should be written.
 * @param count Amount of bytes which should be written.
 * @return Amount of bytes written to the socket.
 */
static ssize_t
sendfn(void *buf, size_t count)
{
	return gnrc_tcp_send(&tcb, buf, count, GNRC_TIMEOUT);
}

int
init_protocol(ipv6_addr_t *remote)
{
	int ret;
	_9pfid *rfid;

	gnrc_tcp_tcb_init(&tcb);
	if ((ret = gnrc_tcp_open_active(&tcb, AF_INET6,
			(uint8_t *)remote, FEUCHT_PORT, 0)))
		return ret;

	_9pinit(&ctx, recvfn, sendfn);
	if ((ret = _9pversion(&ctx)))
		return ret;

	if ((ret = _9pattach(&ctx, &rfid, "glenda", NULL)))
		return ret;

	if ((ret = _9pwalk(&ctx, &hfid, HUMIDITY_FN)) ||
			(ret = _9popen(&ctx, hfid, OWRITE)))
		return ret;

	return 0;
}

int
update_humidity(char *buf, size_t count)
{
	int ret;
	netopt_state_t state;

	state = NETOPT_STATE_IDLE;
	if ((ret = gnrc_netapi_set(netif, NETOPT_STATE,
			0, &state, sizeof(netopt_state_t)) < 0))
		return ret;

	if ((ret = _9pwrite(&ctx, hfid, buf, count)) < 0)
		return ret;

	return 0;
}
