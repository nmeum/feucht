#include <string.h>

#include "coap.h"
#include "feucht.h"

#include "arpa/inet.h"
#include "byteorder.h"

/**
 * CoAP connection context.
 */
static coap_context_t *ctx;

/**
 * CoAP destination address.
 */
static coap_address_t dst;

/**
 * End point for updating humidity values.
 */
#define HUMIDITY_PATH "humidity"

/**
 * PDU size used by CoAP.
 */
#ifndef COAP_DEFAULT_PDU_SIZE
	#define COAP_DEFAULT_PDU_SIZE 64
#endif

int
init_protocol(void)
{
	coap_address_t laddr;

	coap_address_init(&laddr);
	laddr.addr.sin.sin_family = AF_INET6;
	laddr.addr.sin.sin_port = htons(FEUCHT_PORT);
	laddr.addr.sin.sin_addr.s_addr = INADDR_ANY;

	if (!(ctx = coap_new_context(&laddr)))
		return -ENOMEM;

	coap_address_init(&dst);
	dst.addr.sin.sin_family = AF_INET6;
	dst.addr.sin.sin_port = htons(FEUCHT_PORT);
	if (inet_pton(AF_INET6, FEUCHT_HOST, &dst.addr.sin6.sin6_addr) != 1)
		return -EINVAL;

	return 0;
}

int
update_humidity(char *buf, size_t count)
{
	coap_pdu_t *req;

	if (!(req = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_POST,
			coap_new_message_id(ctx), COAP_DEFAULT_PDU_SIZE)))
		return -ENOMEM;

	coap_add_option(req, COAP_OPTION_URI_PATH,
			strlen(HUMIDITY_PATH), (unsigned char*)HUMIDITY_PATH);
	coap_add_data(req, count, (unsigned char*)buf);

	if (COAP_INVALID_TID == coap_send_confirmed(ctx, &dst, req)) {
		coap_delete_pdu(req);
		return -EIO;
	}

	coap_read(ctx);
	coap_dispatch(ctx);

	return 0;
}
