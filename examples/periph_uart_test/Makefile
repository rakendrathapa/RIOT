# name of the application
APPLICATION = periph-uart-int-test

# if no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# Which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# This application uses vtimer module.
USEMODULE += vtimer

# Change this to 0 show invocation lines by default
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
