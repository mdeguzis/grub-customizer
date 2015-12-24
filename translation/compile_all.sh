#!/bin/bash

for i in *.po
do
	msgfmt -c -o `echo $i | grep -o '^[^.]*'`.mo $i
done
