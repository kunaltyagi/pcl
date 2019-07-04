#! /usr/bin/sh

format() {
# don't use a directory with whitespace
local whitelist="2d cuda gpu ml simulation stereo tracking"

local guess_PCL_DIR="$(dirname "$0")/../"
local guess_PCL_DIR=$(cd "${guess_PCL_DIR}"; pwd)
local PCL_DIR="${PCL_DIR:-${guess_PCL_DIR}}"
echo $PCL_DIR

local formatter_versions="9 9.0 8 8.0 7 7.0 6 6.0"
for version in ${formatter_versions}; do
    local guess_formatter="${guess_formatter:-"$(which clang-format-${version})"}"
done
local guess_formatter="${guess_formatter:-"$(which clang-format)"}"
local formatter="${1:-${guess_formatter}}"

if [ ! -f "${formatter}" ]; then
    echo "Could not find a clang-format. Please specify one as the first argument"
    exit 166
fi

local version_string="$("${formatter}" -version | sed "s/^.*clang-format version \([0-9.-]*\).*$/\1/")"

if [ "${version_string}" \< "6" ]; then
    echo "Required version: >6.0.0; Found version: ${version_string}"
    exit 167
fi
exit

for dir in ${whitelist}; do
    path=${PCL_DIR}/${dir}
    if [ ! -d "${path}" ]; then
        # This should not happen. guess_PCL_DIR should be sufficient in most cases
        echo "Could not find ${dir} in ${PCL_DIR}. Is the PCL_DIR env var correct?"
        exit 168
    fi
    # should cuda files be formatted by clang-format? extension: *.cu
    find ${path} -type f -iname *.[ch] -o -iname *.[ch]pp -o -iname *.[ch]xx | \
        xargs -n1 ${formatter} -i -style=file
done
}

format $@

