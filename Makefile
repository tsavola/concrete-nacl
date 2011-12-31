PLATFORM	:= pepper_15

-include config.mk

build:: scons.sh
	@ $(MAKE) -C concrete concrete/prepare O=../core
	@ bash scons.sh

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
	./bootstrap.sh $(SDK) $(PLATFORM)

clean::
	rm -f .sconsign.dblite
