.PHONY: default nocurses
default: nocurses

BUILD_DATE  := $(shell date "+%Y-%m-%dT%H:%M:%S")

LNX_GCC_RUN := gcc -shared -fPIC -O2 -g
WIN_GCC_RUN := gcc -shared -fPIC -O2
MAC_GCC_RUN := MACOSX_DEPLOYMENT_TARGET=10.8 gcc -O2 -bundle -undefined dynamic_lookup -all_load

LNX_COPTS   :=
WIN_COPTS   := -I/mingw64/include/lua5.1 
MAC_COPTS   := -I/usr/local/opt/lua/include/lua5.3 

LNX_LOPTS   := 
WIN_LOPTS   := -lkernel32
MAC_LOPTS   := 

LNX_SO_EXT  := so
WIN_SO_EXT  := dll
MAC_SO_EXT  := so

GCC_RUN     :=
SO_EXT      :=
COPTS       :=
LOPTS       :=

# platforms: LNX, WIN, MAC
# (may be set in sandbox.mk)

PLATFORM    := LNX
LUA_VERSION := 5.4

-include sandbox.mk

GCC_RUN       := $(or $(GCC_RUN),       $($(PLATFORM)_GCC_RUN))
SO_EXT        := $(or $(SO_EXT),        $($(PLATFORM)_SO_EXT))
COPTS         := $(or $(COPTS),         $($(PLATFORM)_COPTS))
LOPTS         := $(or $(LOPTS),         $($(PLATFORM)_LOPTS))

nocurses:
	@mkdir -p build/lua$(LUA_VERSION)/
	$(GCC_RUN) $(COPTS) \
	    -D NOCURSES_VERSION=Makefile"-$(BUILD_DATE)" \
	    main.c  \
	    nocurses_compat.c  \
	    $(LOPTS) \
	    -o build/lua$(LUA_VERSION)/nocurses.$(SO_EXT)
	    

