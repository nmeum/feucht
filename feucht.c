#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdc1000.h"
#include "hdc1000_params.h"

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

	puts("Waiting for address autoconfiguration...");
	/* TODO */

	puts("Configured network interfaces:");
	_netif_config(0, NULL);

	puts("Initialize protocol backend:");
	if ((ret = init_protocol())) {
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
		buf[ret] = '\n';

		if ((ret = update_humidity(buf, ret + 1))) {
			fprintf(stderr, "update_humidity failed: %d\n", ret);
			continue;
		}

		/* TODO sleep */
	}

	/* TODO cleanup */

	return EXIT_SUCCESS;
}
