#!/bin/bash

#free -mh && mkdir fixture && unzip $FIXTURES/assignment2-n-i-rad.zip -d fixture && cp fixture/makefile makefile && cp -r fixture/test/* test/ && make all; dmesg | egrep -i “killed process”

free -mh

echo $FIXTURES

mkdir -p fixture &&
	unzip -o $FIXTURES/*.zip -d fixture &&
	cp fixture/makefile makefile &&
	cp -n fixture/RBTree.hpp RBTree.hpp &&
	cp -n fixture/main.cpp main.cpp &&
	mkdir -p "test" &&
	cp -r fixture/test/* test/
prepResult=$?

if [ $prepResult -ne 0 ]; then
	echo "Failed to create and unzip fixtures"
	echo "Skipping further steps"
else
	make all
	makeResult=$?
	if [ $makeResult -ne 0 ]; then
		echo "Make failed. Checking logs"
		dmesg | egrep -i "killed process"
	else
		echo "Make ran successfully"
	fi
fi
