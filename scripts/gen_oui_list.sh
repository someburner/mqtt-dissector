#!/bin/bash
URL='http://standards-oui.ieee.org/oui.txt'

echo "" > oui_list.txt
curl -s $URL | grep base.*Espressif > oui_list.txt
