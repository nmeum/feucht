# Name of your application
APPLICATION = feucht

# Address of the protocol server
FEUCHT_HOST ?= ::1
CFLAGS += -DFEUCHT_HOST="\"$(FEUCHT_HOST)\""

# Board for which the application should be compiled
BOARD ?= pba-d-01-kw2x

# Disable optional assert(3) checks
CFLAGS += -DNDEBUG

# Change this to 0 show compiler invocation lines
QUIET ?= 1

# Path to the RIOT submodule
RIOTBASE ?= $(CURDIR)/../../vendor/RIOT

# Set SRC manually to select the protocol backend
export NO_AUTO_SRC := 1 # XXX comment me
SRC := feucht.c

# Set modules and files for protocol backend
ifeq (COAP,$(FEUCHT_PROTO))
  SRC += coap.c

  USEPKG += libcoap
else
  ifeq (9P,$(FEUCHT_PROTO))
    SRC += 9p.c

    USEMODULE += gnrc_netdev_default
    USEMODULE += auto_init_gnrc_netif
    USEMODULE += gnrc_ipv6_default
    USEMODULE += gnrc_tcp
  else
    $(error Unknown protocol backend.)
  endif
endif

# Modules for debugging purposes
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell_commands

# Module required for reading from the sensor
USEMODULE += hdc1000

include $(RIOTBASE)/Makefile.include
