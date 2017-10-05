#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "net/ipv6/addr.h"
#include "xtimer.h"
#include "feucht.h"
#include "sema.h"

/* import "ifconfig" shell command, used for printing addresses */
extern int _netif_config(int argc, char **argv);

#ifdef COAP_SEM
extern sema_t coapsem;
#endif

int
main(void)
{
	int n, ret;
	char buf[17];
	ipv6_addr_t remote;

	if (!ipv6_addr_from_str(&remote, FEUCHT_HOST)) {
		fprintf(stderr, "Address '%s' is malformed\n", FEUCHT_HOST);
		return EXIT_FAILURE;
	}

	puts("Waiting for address autoconfiguration...");
	xtimer_sleep(3);

	puts("Configured network interfaces:");
	_netif_config(0, NULL);

	puts("Initialize protocol backend...");
	if ((ret = init_protocol(&remote))) {
		fprintf(stderr, "init_protocol failed: %d\n", ret);
		return EXIT_FAILURE;
	}

#ifdef FEUCHT_RUNS
	for (int n = 1; n <= FEUCHT_RUNS; n++) {
#else
	for (;;) {
#endif
		buf[0] = '1';
		buf[1] = '2';
		buf[2] = '3';
		buf[3] = '4';
		buf[4] = '\n';

		if ((ret = update_humidity(buf, 5))) {
			fprintf(stderr, "update_humidity failed: %d\n", ret);
			continue;
		}

#ifdef COAP_SEM
		if (sema_wait(&coapsem)) {
			fprintf(stderr, "semaphore was destroyed\n");
			break;
		}
#endif

		if (FEUCHT_INTERVAL > 0)
			xtimer_sleep(FEUCHT_INTERVAL);
	}

	free_protocol();
	puts("DONE");

	return EXIT_SUCCESS;
}
