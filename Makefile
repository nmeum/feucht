# Name of your application
APPLICATION = feucht
INCLUDES += -I$(CURDIR)/include

# Board for which the application should be compiled
BOARD ?= pba-d-01-kw2x

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

# Module required for reading from the sensor
USEMODULE += hdc1000

# Backend specific files and modules
ifeq (COAP,$(FEUCHT_PROTO))
  DIRS += coap
  FEUCHT_PORT ?= 5683

  USEMODULE += gcoap
  USEMODULE += coap
else
  ifeq (9P,$(FEUCHT_PROTO))
    DIRS += 9p
    FEUCHT_PORT ?= 5640

    USEPKG += ninenano
    USEMODULE += gnrc_tcp
    USEMODULE += 9p
  else
    $(error Unknown protocol backend.)
  endif
endif

# Address and port of the protocol server
CFLAGS += -DFEUCHT_PORT=$(FEUCHT_PORT)
ifneq (,$(FEUCHT_HOST))
  CFLAGS += -DFEUCHT_HOST="\"$(FEUCHT_HOST)\""
endif

include $(RIOTBASE)/Makefile.include
