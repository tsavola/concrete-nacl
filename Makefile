-include config.mk

define make-concrete
	CFLAGS="-m$(2)" \
	MAKEFILES="$(PWD)/concrete.mk $(3)" \
		$(MAKE) -C concrete O="$(PWD)/$(1)_x86_$(2)/concrete" SDK="$(SDK)" concrete-static
endef

build:: scons.sh concrete-dbg-32 concrete-dbg-64 concrete-opt-32 concrete-opt-64
	@ bash scons.sh

concrete-dbg-32::
	$(call make-concrete,dbg,32,debug.mk)

concrete-dbg-64::
	$(call make-concrete,dbg,64,debug.mk)

concrete-opt-32::
	$(call make-concrete,opt,32,optimize.mk)

concrete-opt-64::
	$(call make-concrete,opt,64,optimize.mk)

test:: examples/httpd.py
	@ cd examples && ./httpd.py &
	@ rm -rf .chrome/cache
	@ google-chrome \
		--user-data-dir="$(PWD)/.chrome" \
		--disk-cache-dir="$(PWD)/.chrome/cache" \
		--disk-cache-size=1 \
		"http://localhost:5103/concrete.html"
	@ wget -q -O- "http://localhost:5103/?quit=1"

scons.sh examples/httpd.py:
	./bootstrap.sh $(SDK)

clean::
	rm -f *.nexe *.nmf .sconsign.dblite
	rm -rf dbg_x86_32 dbg_x86_64 opt_x86_32 opt_x86_64
