# Name of your application
APPLICATION = feucht
INCLUDES += -I$(CURDIR)/include

# Board for which the application should be compiled
BOARD ?= native

# Disable optional assert(3) checks
CFLAGS += -DNDEBUG

# Change this to 0 show compiler invocation lines
QUIET ?= 1

# Modules required for both protocol backends
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += xtimer

# Modules for debugging purposes
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell_commands

# Backend specific files and modules
ifeq (COAP,$(FEUCHT_PROTO))
  DIRS += coap
  FEUCHT_PORT ?= 5683

  USEMODULE += gcoap
  USEMODULE += coap
  USEMODULE += sema

  CFLAGS += -DCOAP_SEM -DGCOAP_RESEND_BUFS_MAX=30 -DGCOAP_REQ_WAITING_MAX=30
else
  ifeq (9P,$(FEUCHT_PROTO))
    DIRS += 9p
    FEUCHT_PORT ?= 5640

    USEPKG += ninenano
    USEMODULE += gnrc_tcp
    USEMODULE += 9p

    CFLAGS += -D_9P_MSIZE=127 -D_9P_MAXFIDS=2
  else
    $(error Unknown protocol backend.)
  endif
endif

# Address and port of the protocol server
CFLAGS += -DFEUCHT_PORT=$(FEUCHT_PORT)
ifneq (,$(FEUCHT_HOST))
  CFLAGS += -DFEUCHT_HOST="\"$(FEUCHT_HOST)\""
endif

# IEEE 802.15.4 channel to use
ifneq (,$(FEUCHT_CHANNEL))
  CFLAGS += -DIEEE802154_DEFAULT_CHANNEL="$(FEUCHT_CHANNEL)"
endif

CFLAGS += -DFEUCHT_RUNS=15 -DFEUCHT_INTERVAL=0

include $(RIOTBASE)/Makefile.include
