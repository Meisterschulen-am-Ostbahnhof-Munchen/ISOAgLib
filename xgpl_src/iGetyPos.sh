#!/bin/sh

for InterfaceSouce in `find . \( -name "i*.h" -o -name "i*.hpp" \) -and -not -path "*/impl/*"` ; do
	echo Datei $InterfaceSouce

	perl -p -i -e 's/GetyPos_c/iGetyPos_c/g' $InterfaceSouce
	perl -p -i -e 's/iiGetyPos_c/iGetyPos_c/g' $InterfaceSouce
#	exit 0
done
