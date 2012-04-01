PYTHON2		?= python2
PYTHON		?= python3
CHROME		?= google-chrome

-include config.mk

CPPFLAGS	+= -I. -I$(O)/src -Iconcrete/boost -DCONCRETE_PORTABLE_BYTEORDER=true
CFLAGS		+= -g -Wall
CXXFLAGS	+= -std=gnu++0x

BINARIES	:= nacl
OTHERS		:= concrete
TESTS		:= test

build: nacl
nacl: concrete
test: nacl

export NACL_SDK_ROOT
export PYTHON2
export PYTHON
export CHROME

include build/project.mk

dist::
	rm -rf dist
	mkdir -p dist/debug
	cp test/index.html test/code.hex dist/
	cp -r debug/*.nmf debug/bin debug/lib32 debug/lib64 dist/debug/
