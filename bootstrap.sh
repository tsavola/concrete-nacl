#!/bin/sh

SDK="$1"

cat "$SDK/project_templates/scons" | sed "s,<NACL_SDK_ROOT>,$1,g" > scons.sh
cp "$SDK/examples/httpd.py" examples/
git submodule update --init
(cd concrete && ./bootstrap.sh)
