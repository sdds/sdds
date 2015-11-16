#!/usr/bin/env sh
SCRIPT_DIR="../scripts"
GSL_DIR="$SCRIPT_DIR"/gsl

#  Remove gsl if it has not been properly installed
if [ ! -e "$GSL_DIR/src/gsl" ]; then
    echo "Remove GSL $GSL_DIR"
    rm -rf $GSL_DIR
fi
#  Build and Install gsl
if [ ! -d $GSL_DIR ]; then
    cd $SCRIPT_DIR
    ./install_gsl.sh
    cd -
fi
#  Generate code
export PATH=$PATH:$SCRIPT_DIR:$GSL_DIR/src
gsl examples.xml
