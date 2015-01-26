#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: ${0} <pluto-file.root> <output-file.root> [optional other paramters of A2 geant]"
    exit 1;
fi

INFILE=$(readlink -f "${1}")
OUTFILE=$(readlink -f "${2}")
PROG=$(readlink -f "${0}")
BASE=$(dirname "${PROG}")
shift 2
cd "${BASE}"
exec ./A2 --mac=sim_pluto.mac --if="${INFILE}" --of="${OUTFILE}" $* 
