#!/bin/bash -e

if [ $# -lt 2 ]; then
    echo "Usage: ${0} <pluto-file.root> <output-file.root> [macro regex replacements] [A2 geant parameters]"
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
REGEX_PIPES=""
for arg in "$@"
do
    if [[ $arg =~ ^s.* ]]; then
        # arg starts with s, looks like replacement regex
        REGEX_PIPES+=" | $BASE/modifyMacro.pl '$arg'"
        N_REGEXES=$(($N_REGEXES+1))
    fi
done

DETECTOR_FILE="$BASE/macros/DetectorSetup.mac"
SIMU_FILE="$BASE/sim_pluto.mac"
if [[ $N_REGEXES > 0 ]]; then
    shift $N_REGEXES # get rid of regex args
    MACROS_TMP_DIR="$BASE/macros_tmp"
    # runGeant.sh is typically shared among users
    # so provide global tmp dir for temporary files
    if [ ! -d "$MACROS_TMP_DIR" ]; then
        mkdir "$MACROS_TMP_DIR"
        chmod 1777 "$MACROS_TMP_DIR"
    fi
    TMP_DETECTOR_FILE=$(mktemp $BASE/macros_tmp/DetectorSetup.XXXXXXXXXXX.mac)
    TMP_SIMU_FILE=$(mktemp $BASE/macros_tmp/sim_pluto.XXXXXXXXXXX.mac)
    trap "rm -f $TMP_DETECTOR_FILE $TMP_SIMU_FILE" EXIT
    eval "cat $DETECTOR_FILE $REGEX_PIPES > $TMP_DETECTOR_FILE"
    eval "cat $SIMU_FILE $REGEX_PIPES > $TMP_SIMU_FILE"
    DETECTOR_FILE=$TMP_DETECTOR_FILE
    SIMU_FILE=$TMP_SIMU_FILE
fi

cd "${BASE}"
./A2 --mac="${SIMU_FILE}" --if="${INFILE}" --of="${OUTFILE}" --det="${DETECTOR_FILE}" $*
