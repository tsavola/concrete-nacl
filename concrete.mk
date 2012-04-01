include build/common.mk

build::
	$(QUIET) $(MAKE) -C concrete concrete/prepare O=../$(O)
