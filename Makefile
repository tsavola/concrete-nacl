PLATFORM	:= pepper_16
PYTHON		:= python3
J		:= 1

-include config.mk

build:: scons.sh examples/code.hex
	@ $(MAKE) -C concrete concrete/prepare O=../core
	@ bash scons.sh -j$(J)

test:: examples/httpd.py
	@ cd examples && ./httpd.py &
	@ rm -rf .chrome/cache
	@ google-chrome \
		--user-data-dir="$(PWD)/.chrome" \
		--disk-cache-dir="$(PWD)/.chrome/cache" \
		--disk-cache-size=1 \
		"http://localhost:5103/example.html"
	@ wget -q -O- "http://localhost:5103/?quit=1"

examples/code.hex: examples/code.py
	$(PYTHON) -c "import marshal, base64; x = base64.b16encode(marshal.dumps(compile(open('examples/code.py').read(), 'examples/code.py', 'exec'))); open('"$@"', 'wb').write(x)"

scons.sh examples/httpd.py:
	./bootstrap.sh $(SDK) $(PLATFORM)

dist::
	rm -rf dist
	mkdir -p dist
	cp concrete_???_??_dbg.nexe concrete_dbg.nmf examples/code.hex dist/
	cp example.html dist/index.html

clean::
	rm -rf core
	rm -f concrete.nmf concrete_dbg.nmf
	rm -f concrete_x86_32.nexe concrete_x86_32_dbg.nexe
	rm -f concrete_x86_64.nexe concrete_x86_64_dbg.nexe
	rm -rf dbg_x86_32 dbg_x86_64 opt_x86_32 opt_x86_64
	rm -f .sconsign.dblite
	rm -f scons.sh examples/code.hex examples/httpd.py
	rm -rf dist
