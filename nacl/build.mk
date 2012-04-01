OSNAME		:= $(shell $(PYTHON2) $(NACL_SDK_ROOT)/tools/getos.py)
CREATENMF	:= $(PYTHON2) $(NACL_SDK_ROOT)/tools/create_nmf.py
TOOLCHAIN	:= $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_x86_glibc

NAME		:= concrete
BINARIES	:= $(patsubst %,bin/$(NAME)_x86_%.nexe,32 64)
NMF		:= $(NAME).nmf

O_BINARIES	:= $(patsubst %,$(O)/%,$(BINARIES))
O_NMF		:= $(O)/$(NMF)

export TOOLCHAIN

include build/common.mk

build:: $(O_BINARIES) $(O_NMF)

$(O_NMF): $(O_BINARIES)
	$(call echo,Create,$@)
	$(QUIET) cd $(O) && $(CREATENMF) \
		-o $(NMF) \
		-D $(TOOLCHAIN)/x86_64-nacl/bin/objdump \
		-L $(TOOLCHAIN)/x86_64-nacl/lib32 \
		-L $(TOOLCHAIN)/x86_64-nacl/lib \
		-s . \
		$(BINARIES)

$(O)/bin/$(NAME)_x86_32.nexe::
	$(QUIET) $(MAKE) -f nacl/build-binary.mk BITS=32

$(O)/bin/$(NAME)_x86_64.nexe::
	$(QUIET) $(MAKE) -f nacl/build-binary.mk BITS=64
