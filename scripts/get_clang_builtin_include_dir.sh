#!/bin/bash
if [ "$(uname)" == 'Darwin' ] || [ "$(expr substr $(uname -s) 1 5)" == 'Linux' ]; then
	# macOS, Linux
	version=`$1 --version | head -1 | sed 's/^.*[^0-9] \([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`
	basepath="$(dirname $(dirname $(which $1)))"
	echo ${basepath}/lib/clang/${version}/include
else
	echo "Your platform ($(uname -a)) is not supported."
	exit 1
fi

