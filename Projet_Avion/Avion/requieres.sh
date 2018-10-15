#!/bin/bash

#apt-get update
#apt-get install -y make
#apt-get install -y gcc
cd /src/Avion
make
while true; do ./avion.out; done

