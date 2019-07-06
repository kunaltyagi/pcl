#! /usr/bin/sh

format() {
    # don't use a directory with whitespace
    local whitelist="2d cuda gpu ml simulation stereo tracking"

    local PCL_DIR="${2}"
    local formatter="${1}"

    if [ ! -f "${formatter}" ]; then
        echo "Could not find a clang-format. Please specify one as the first argument"
        exit 166
    fi

    # check for self
    if [ ! -f "${PCL_DIR}/dev/format.sh" ]; then
        echo "Please ensure that PCL_SOURCE_DIR is passed as the second argument"
        exit 166
    fi

    for dir in ${whitelist}; do
        path=${PCL_DIR}/${dir}
        find ${path} -type f -iname *.[ch] -o -iname *.[ch]pp -o -iname *.[ch]xx \
            -iname *.cu | xargs -n1 ${formatter} -i -style=file
    done
}

format $@
