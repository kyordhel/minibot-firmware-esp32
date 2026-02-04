#!/usr/bin/env bash

if [[ "${IDF_PATH}" == "" ]]; then
	echo -e "\e[31m[ERROR]\e[0m ESP-IDF not found! Did you forget to source it?"

	echo ""
	echo -en "\e[1;97mNote:\e[0m "
	echo "The README.md file contains detailed information"
	echo "on how to download, install and source the ESP-IDF"
	echo "required to build the minibot firmware."
	exit -2
fi

echo -e "\e[32m[OK]\e[0m ESP-IDF found at \e[1m${IDF_PATH}\e[0m"
cd build
idf.py build -C ..
