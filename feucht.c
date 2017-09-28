#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hdc1000.h"
#include "hdc1000_params.h"

#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/netopt.h"

#include "periph/gpio.h"
#include "xtimer.h"
#include "feucht.h"

/**
 * Pid of the thread for the radio module.
 */
kernel_pid_t netif;

/**
 * HDC1000 device.
 */
static hdc1000_t hdc;

/* import "ifconfig" shell command, used for printing addresses */
extern int _netif_config(int argc, char **argv);

int
main(void)
{
	int pin, ret;
	char buf[17];
	size_t i, nifs;
	int16_t temp, hum;
	ipv6_addr_t remote;
	kernel_pid_t ifs[GNRC_NETIF_NUMOF];

	pin = GPIO_PIN(PORT_E, 2);
	if (gpio_init(pin, GPIO_OUT) == -1) {
		fprintf(stderr, "Couldn't initialize the GPIO pin\n");
		return EXIT_FAILURE;
	}
	gpio_clear(pin);

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

	nifs = gnrc_netif_get(ifs);
	printf("Found %zu network interfaces.\n", nifs);
	for (i = 1; i <= nifs; i++)
		printf("Network interface %zu has pid %d\n", i, ifs[i]);
	netif = ifs[0];

	puts("Initialize protocol backend...");
	if ((ret = init_protocol(&remote))) {
		fprintf(stderr, "init_protocol failed: %d\n", ret);
		return EXIT_FAILURE;
	}

	for (;;) {
		gpio_set(pin);

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

		xtimer_sleep(1);
		gpio_clear(pin);

		xtimer_sleep(FEUCHT_INTERVAL);
	}

	/* TODO cleanup */

	return EXIT_SUCCESS;
}
