#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: ${0} <pluto-file.root> <output-file.root>"
    exit 1;
fi

INFILE=$(readlink -f "${1}")
OUTFILE=$(readlink -f "${2}")
PROG=$(readlink -f "${0}")
BASE=$(dirname "${PROG}")
cd "${BASE}"
exec ./A2 --mac=sim_pluto.mac --if="${INFILE}" --of="${OUTFILE}"
