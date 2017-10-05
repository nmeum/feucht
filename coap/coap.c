#include <string.h>
#include <stdint.h>

#include "net/gcoap.h"
#include "net/ipv6/addr.h"
#include "feucht.h"
#include "sema.h"

#include "arpa/inet.h"
#include "byteorder.h"

/**
 * CoAP destination endpoint.
 */
static sock_udp_ep_t ep;

#ifdef COAP_SEM
/**
 * CoAP semaphore to detect when all ACKs were received in main().
 */
sema_t coapsem = SEMA_CREATE_LOCKED();
#endif

/**
 * End point for updating humidity values.
 */
#define HUMIDITY_PATH "/humidity"

static void _resp_handler(unsigned req_state, coap_pkt_t* pdu,
		 sock_udp_ep_t *remote)
{
	(void)remote;

	if (req_state == GCOAP_MEMO_TIMEOUT) {
		printf("gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
		goto ret;
	} else if (req_state == GCOAP_MEMO_ERR) {
		puts("gcoap: error in response");
		goto ret;
	}

	if (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
		puts("Humidity has been updated!");
	else
		puts("gcoap: error response class");

ret:
	if (sema_post(&coapsem))
		printf("gcoap: sema would overflow\n");
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

void
free_protocol(void)
{
	return;
}

int
update_humidity(char *buf, size_t count)
{
	ssize_t len;
	coap_pkt_t pdu;
	uint8_t pbuf[GCOAP_PDU_BUF_SIZE];

	if (gcoap_req_init(&pdu, pbuf, GCOAP_PDU_BUF_SIZE, 2, HUMIDITY_PATH))
		return -ENOMEM;
	memcpy(pdu.payload, buf, count);

	coap_hdr_set_type(pdu.hdr, COAP_TYPE_CON);
	if ((len = gcoap_finish(&pdu, count, COAP_FORMAT_TEXT)) < 0)
		return len;

	if (!gcoap_req_send2(pbuf, len, &ep, _resp_handler))
		return -EIO;

	return 0;
}
