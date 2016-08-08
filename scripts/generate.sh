#!/usr/bin/env sh

SELF_SYM=$(readlink $0)
SCRIPT_DIR=$(dirname -- "$SELF_SYM")
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
gsl sdds.xml
