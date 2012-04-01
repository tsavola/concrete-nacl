include build/common.mk

build:: test/code.hex

check:: test/httpd.py
	$(call echo,Start,test/httpd.py)
	$(QUIET) cd test && ./httpd.py &

	$(call echo,Browse,test/index.html)
	$(QUIET) rm -rf .chrome/cache
	$(QUIET) $(CHROME) \
		--user-data-dir="$(PWD)/.chrome" \
		--disk-cache-dir="$(PWD)/.chrome/cache" \
		--disk-cache-size=1 \
		--enable-nacl \
		"http://localhost:5103/index.html"

	$(call echo,Stop,test/httpd.py)
	$(QUIET) wget -q -O- "http://localhost:5103/?quit=1"

test/code.hex: test/code.py
	$(call echo,Encode,$@)
	$(QUIET) $(PYTHON) -c "import marshal, base64; x = base64.b16encode(marshal.dumps(compile(open('test/code.py').read(), 'test/code.py', 'exec'))); open('"$@"', 'wb').write(x)"

test/httpd.py:
	$(call echo,Install,$@)
	$(QUIET) cp $(NACL_SDK_ROOT)/test/httpd.py $@
	$(QUIET) chmod +x $@
