#!/bin/bash

INFILE=$(readlink -f "${1}")
OUTFILE=$(readlink -f "${2}")
LOGFILE="${OUTFILE}.log"
PROG=$(readlink -f "${0}")
BASE=$(dirname "${PROG}")
cd "${BASE}"
exec ./A2 --mac=sim_pluto.mac --if="${INFILE}" --of="${OUTFILE}" 2>1 > "${LOGFILE}"
