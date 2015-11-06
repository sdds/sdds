#!/usr/bin/env sh
if [ ! -d "../../scripts/gsl" ]; then
cd ../../scripts
./install_gsl.sh
cd -
fi
export PATH=$PATH:$PWD/../../scripts:$PWD/../../scripts/gsl/src
gsl sdds.xml
