# Name of your application
APPLICATION = feucht

# Board for which the application should be compiled
BOARD ?= pba-d-01-kw2x

# Disable optional assert(3) checks
CFLAGS += -DNDEBUG

# Change this to 0 show compiler invocation lines
QUIET ?= 1

# Path to the RIOT submodule
RIOTBASE ?= $(CURDIR)/../../vendor/RIOT

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

# Set SRC manually to select the protocol backend
#export NO_AUTO_SRC := 1 # XXX comment me
SRC := feucht.c

# Backend specific files and modules
ifeq (COAP,$(FEUCHT_PROTO))
  SRC += coap.c
  FEUCHT_PORT ?= 5683

  USEMODULE += gcoap
else
  ifeq (9P,$(FEUCHT_PROTO))
    SRC += 9p.c
    FEUCHT_PORT ?= 5640

    USEMODULE += gnrc_tcp
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
