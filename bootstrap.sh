#!/bin/sh

SDK="$1"
PLATFORM="$2"

cat "$SDK/$PLATFORM/project_templates/scons" | sed -e "s,<NACL_SDK_ROOT>,$SDK,g" -e "s,<NACL_PLATFORM>,$PLATFORM,g" > scons.sh
cp "$SDK/$PLATFORM/examples/httpd.py" examples/
git submodule update --init
(cd concrete && ./bootstrap.sh)
