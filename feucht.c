#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hdc1000.h"
#include "hdc1000_params.h"

#include "net/ipv6/addr.h"
#include "xtimer.h"
#include "feucht.h"

/**
 * HDC1000 device.
 */
static hdc1000_t hdc;

/* import "ifconfig" shell command, used for printing addresses */
extern int _netif_config(int argc, char **argv);

int
main(void)
{
	int ret;
	char buf[17];
	int16_t temp, hum;
	ipv6_addr_t remote;

	if ((ret = hdc1000_init(&hdc, &hdc1000_params[0])) != HDC1000_OK) {
		fprintf(stderr, "Couldn't initialize HDC1000: %d\n", ret);
		return EXIT_FAILURE;
	}

	if (!ipv6_addr_from_str(&remote, FEUCHT_HOST)) {
		fprintf(stderr, "Address '%s' is malformed\n", FEUCHT_HOST);
		return EXIT_FAILURE;
	}

	puts("Waiting for address autoconfiguration...");
	xtimer_sleep(3);

	puts("Configured network interfaces:");
	_netif_config(0, NULL);

	puts("Initialize protocol backend:");
	if ((ret = init_protocol(&remote))) {
		fprintf(stderr, "init_protocol failed: %d\n", ret);
		return EXIT_FAILURE;
	}

	for (;;) {
		if (hdc1000_read(&hdc, &temp, &hum) != HDC1000_OK) {
			fprintf(stderr, "hdc1000_read failed\n");
			continue;
		}

		memset(buf, '\0', sizeof(buf));
		ret = snprintf(buf, sizeof(buf) - 1, "%d", hum);

		assert(ret < sizeof(buf));
		buf[ret] = '\n';

		if ((ret = update_humidity(buf, ret + 1))) {
			fprintf(stderr, "update_humidity failed: %d\n", ret);
			continue;
		}

		xtimer_sleep(FEUCHT_INTERVAL);
	}

	/* TODO cleanup */

	return EXIT_SUCCESS;
}
