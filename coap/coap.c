#include <string.h>
#include <stdint.h>

#include "net/gcoap.h"
#include "net/ipv6/addr.h"
#include "net/netopt.h"

#include "feucht.h"
#include "arpa/inet.h"
#include "byteorder.h"
#include "kernel_types.h"

extern kernel_pid_t netif;

/**
 * CoAP destination endpoint.
 */
static sock_udp_ep_t ep;

/**
 * End point for updating humidity values.
 */
#define HUMIDITY_PATH "/humidity"

static void _resp_handler(unsigned req_state, coap_pkt_t* pdu,
		 sock_udp_ep_t *remote)
{
	int ret;
	netopt_state_t state;

	(void)remote;

	state = NETOPT_STATE_SLEEP;
	if ((ret = gnrc_netapi_set(netif, NETOPT_STATE,
			0, &state, sizeof(netopt_state_t)) < 0))
		return;

	if (req_state == GCOAP_MEMO_TIMEOUT) {
		fprintf(stderr, "gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
		return;
	} else if (req_state == GCOAP_MEMO_ERR) {
		fprintf(stderr, "gcoap: error in response\n");
		return;
	}

	char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
		? "Success" : "Error";
	printf("gcoap: response %s\n", class_str);
}

int
init_protocol(ipv6_addr_t *remote)
{
	ep.family = AF_INET6;
	ep.netif  = SOCK_ADDR_ANY_NETIF;
	ep.port   = FEUCHT_PORT;

	memcpy(&ep.addr.ipv6[0], &remote->u8[0], sizeof(remote->u8));
	return 0;
}

int
update_humidity(char *buf, size_t count)
{
	int ret;
	ssize_t len;
	coap_pkt_t pdu;
	netopt_state_t state;
	uint8_t pbuf[GCOAP_PDU_BUF_SIZE];

	if (gcoap_req_init(&pdu, pbuf, GCOAP_PDU_BUF_SIZE, 2, HUMIDITY_PATH))
		return -ENOMEM;
	memcpy(pdu.payload, buf, count);

	coap_hdr_set_type(pdu.hdr, COAP_TYPE_CON);
	if ((len = gcoap_finish(&pdu, count, COAP_FORMAT_TEXT)) < 0)
		return len;

	/* The kw2xrf driver returns sizeof(netopt_state_t) on success. */
	state = NETOPT_STATE_IDLE;
	if ((ret = gnrc_netapi_set(netif, NETOPT_STATE,
			0, &state, sizeof(netopt_state_t)) < 0))
		return ret;

	if (!gcoap_req_send2(pbuf, len, &ep, _resp_handler))
		return -EIO;

	return 0;
}
