#!/bin/sh

SCRIPT_DIR=$(dirname $0)
REPO_DIR="${SCRIPT_DIR}/.."
BIN_DIR="${REPO_DIR}/bin"
TESTS_ENV="${SCRIPT_DIR}/tests.env"

MECA_OPTS="-r 0xDEADBEEFCAFEBABE -e 22 -w 8"

# $1 == input
# $2 == output
call_meca() {
    $BIN_DIR/meca ${MECA_OPTS} -i ${REPO_DIR}/$1 -o ${REPO_DIR}/$2
}

if [ -f $TESTS_ENV ]; then
    . $TESTS_ENV
    call_meca Makefile meca.enc
    call_meca meca.enc meca.dec
    cat "${REPO_DIR}/meca.dec"
else
    echo "error: failed to source ${TESTS_ENV}"
fi
