#!/bin/bash

sudo apt-get install git-core libmxml-dev build-essential
git clone git://github.com/SquidMan/benzin.git

cd benzin

make clean
make

./benzin
