#!/bin/bash -e

if [ $# -lt 2 ]; then
    echo "Usage: ${0} <pluto-file.root> <output-file.root> [DetectorMacro regex replacements] [A2 geant parameters]"
    exit 1;
fi

INFILE=$(readlink -f "${1}")
OUTFILE=$(readlink -f "${2}")
PROG=$(readlink -f "${0}")
BASE=$(dirname "${PROG}")
# remove infile/outfile from argument list
shift 2

# figure out if there are replacements in the arguments
N_REGEXES=0
DETECTOR_FILE="$BASE/macros/DetectorSetup.mac"
REGEX_CMD="cat $DETECTOR_FILE"
for arg in "$@"
do
    if [[ $arg =~ ^s.* ]]; then
        # arg starts with s, looks like replacement regex
        REGEX_CMD+=" | $BASE/modifyMacro.pl '$arg'"
        ((N_REGEXES++))
    fi
done

if [[ $N_REGEXES > 0 ]]; then
    shift $N_REGEXES # get rid of regex args
    MACROS_TMP_DIR="$BASE/macros_tmp"
    # runGeant.sh is typically shared among users
    # so provide
    if [ ! -d "$MACROS_TMP_DIR" ]; then
        mkdir "$MACROS_TMP_DIR"
        chmod 1777 "$MACROS_TMP_DIR"
    fi
    DETECTOR_FILE=$(mktemp $BASE/macros_tmp/DetectorSetup.XXXXXXXXXXX.mac)
    trap "rm -f $DETECTOR_FILE" EXIT
    REGEX_CMD+=" > $DETECTOR_FILE"
    eval $REGEX_CMD
fi

cd "${BASE}"
./A2 --mac=sim_pluto.mac --if="${INFILE}" --of="${OUTFILE}" --det="${DETECTOR_FILE}" $*
