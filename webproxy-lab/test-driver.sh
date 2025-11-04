#!/bin/bash
#
# driver-debug-preserve.sh - Debug version that preserves ALL test files
#
#     usage: ./driver-debug-preserve.sh
#

# Point values
MAX_BASIC=40
MAX_CONCURRENCY=15
MAX_CACHE=15

# Various constants
HOME_DIR=`pwd`
PROXY_DIR="./.proxy"
NOPROXY_DIR="./.noproxy"
DEBUG_DIR="./.debug-$(date +%s)"  # Timestamped debug directory
TIMEOUT=5
MAX_RAND=63000
PORT_START=1024
PORT_MAX=65000
MAX_PORT_TRIES=10

# List of text and binary files for the basic test
BASIC_LIST="home.html
            csapp.c
            tiny.c
            godzilla.jpg
            tiny"

# List of text files for the cache test
CACHE_LIST="tiny.c
            home.html
            csapp.c"

# The file we will fetch for various tests
FETCH_FILE="home.html"

#####
# Helper functions
#

#
# download_proxy - download a file from the origin server via the proxy
# usage: download_proxy <testdir> <filename> <origin_url> <proxy_url>
#
function download_proxy {
    cd $1
    echo "   [DEBUG] Running: curl --max-time ${TIMEOUT} --proxy $4 --output $2 $3" >> ${HOME_DIR}/${DEBUG_DIR}/curl_commands.log
    curl --max-time ${TIMEOUT} --proxy $4 --output $2 $3 -v 2>> ${HOME_DIR}/${DEBUG_DIR}/curl_verbose.log
    local exit_code=$?
    echo "   [DEBUG] Exit code: ${exit_code}" >> ${HOME_DIR}/${DEBUG_DIR}/curl_commands.log
    if (( exit_code == 28 )); then
        echo "Error: Fetch timed out after ${TIMEOUT} seconds"
    elif (( exit_code != 0 )); then
        echo "Error: Fetch failed with exit code ${exit_code}"
    fi
    cd $HOME_DIR
    return $exit_code
}

#
# download_noproxy - download a file directly from the origin server
# usage: download_noproxy <testdir> <filename> <origin_url>
#
function download_noproxy {
    cd $1
    curl --max-time ${TIMEOUT} --output $2 $3
    local exit_code=$?
    if (( exit_code == 28 )); then
        echo "Error: Fetch timed out after ${TIMEOUT} seconds"
    elif (( exit_code != 0 )); then
        echo "Error: Fetch failed with exit code ${exit_code}"
    fi
    cd $HOME_DIR
    return $exit_code
}

#
# save_test_files - Save files to debug directory for inspection
#
function save_test_files {
    local test_name=$1
    local file=$2

    # Create subdirectory for this test
    mkdir -p ${DEBUG_DIR}/${test_name}

    # Copy files if they exist
    if [ -f "${PROXY_DIR}/${file}" ]; then
        cp "${PROXY_DIR}/${file}" "${DEBUG_DIR}/${test_name}/proxy_${file}"
        echo "   Saved: ${DEBUG_DIR}/${test_name}/proxy_${file}"
    fi

    if [ -f "${NOPROXY_DIR}/${file}" ]; then
        cp "${NOPROXY_DIR}/${file}" "${DEBUG_DIR}/${test_name}/direct_${file}"
        echo "   Saved: ${DEBUG_DIR}/${test_name}/direct_${file}"
    fi
}

#
# clear_dirs - Clear the download directories
#
function clear_dirs {
    rm -rf ${PROXY_DIR}/*
    rm -rf ${NOPROXY_DIR}/*
}

#
# wait_for_port_use - Spins until the TCP port number passed as an
#     argument is actually being used. Times out after 5 seconds.
#
function wait_for_port_use() {
    local port="$1"
    local timeout_count=0

    while true
    do
        python3 - "$port" <<'PY'
import socket
import sys

port = int(sys.argv[1])
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.settimeout(0.5)
    try:
        sock.connect(("127.0.0.1", port))
    except OSError:
        sys.exit(1)
sys.exit(0)
PY
        if [ $? -eq 0 ]; then
            return 0
        fi

        timeout_count=$((timeout_count + 1))
        if [ "${timeout_count}" == "${MAX_PORT_TRIES}" ]; then
            kill -ALRM $$
        fi

        sleep 1
    done
}

#
# free_port - returns an available unused TCP port
#
function free_port {
    python3 - <<PY
import random
import socket
import sys

PORT_START = ${PORT_START}
PORT_MAX = ${PORT_MAX}
MAX_TRIES = ${MAX_PORT_TRIES} * 100

rand = random.SystemRandom()
for _ in range(MAX_TRIES):
    port = rand.randint(PORT_START, PORT_MAX)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        try:
            sock.bind(("127.0.0.1", port))
        except OSError:
            continue
        print(port)
        sys.exit(0)

print(-1)
PY
}

#
# show_file_info - Show information about fetched files
#
function show_file_info {
    local proxy_file=$1
    local direct_file=$2

    echo "   File sizes:"
    if [ -f "$proxy_file" ]; then
        echo "     Proxy:  $(ls -lh "$proxy_file" | awk '{print $5}')"
    else
        echo "     Proxy:  FILE NOT FOUND"
    fi

    if [ -f "$direct_file" ]; then
        echo "     Direct: $(ls -lh "$direct_file" | awk '{print $5}')"
    else
        echo "     Direct: FILE NOT FOUND"
    fi

    # Show MD5 checksums
    echo "   MD5 checksums:"
    if [ -f "$proxy_file" ]; then
        echo "     Proxy:  $(md5sum "$proxy_file" | awk '{print $1}')"
    fi
    if [ -f "$direct_file" ]; then
        echo "     Direct: $(md5sum "$direct_file" | awk '{print $1}')"
    fi
}

#
# show_diff - Show detailed differences between files
#
function show_diff {
    local proxy_file=$1
    local direct_file=$2
    local filename=$3

    echo "   === SHOWING DIFFERENCES ==="

    # Check if files are text or binary
    if file "$direct_file" | grep -q text; then
        echo "   Text file - showing first 30 lines of diff:"
        diff -u "$direct_file" "$proxy_file" | head -30
    else
        echo "   Binary file - using hexdump comparison (first 512 bytes):"
        echo "   Direct file hexdump:"
        hexdump -C "$direct_file" | head -20
        echo "   Proxy file hexdump:"
        hexdump -C "$proxy_file" | head -20
    fi

    echo "   =========================="
}

#######
# Main
#######

# Create debug directory
mkdir -p ${DEBUG_DIR}
echo "Debug files will be saved to: ${DEBUG_DIR}/"
echo ""

# Kill any stray proxies or tiny servers owned by this user
killall -q proxy tiny nop-server.py 2> /dev/null

# Make sure we have a Tiny directory
if [ ! -d ./tiny ]
then
    echo "Error: ./tiny directory not found."
    exit
fi

# If there is no Tiny executable, then try to build it
if [ ! -x ./tiny/tiny ]
then
    echo "Building the tiny executable."
    (cd ./tiny; make)
    echo ""
fi

# Make sure we have all the Tiny files we need
if [ ! -x ./tiny/tiny ]
then
    echo "Error: ./tiny/tiny not found or not an executable file."
    exit
fi
for file in ${BASIC_LIST}
do
    if [ ! -e ./tiny/${file} ]
    then
        echo "Error: ./tiny/${file} not found."
        exit
    fi
done

# Make sure we have an existing executable proxy
if [ ! -x ./proxy ]
then
    echo "Error: ./proxy not found or not an executable file. Please rebuild your proxy and try again."
    exit
fi

# Make sure we have an existing executable nop-server.py file
if [ ! -x ./nop-server.py ]
then
    echo "Error: ./nop-server.py not found or not an executable file."
    exit
fi

# Create the test directories if needed
if [ ! -d ${PROXY_DIR} ]
then
    mkdir ${PROXY_DIR}
fi

if [ ! -d ${NOPROXY_DIR} ]
then
    mkdir ${NOPROXY_DIR}
fi

# Add a handler to generate a meaningful timeout message
trap 'echo "Timeout waiting for the server to grab the port reserved for it"; kill $$' ALRM

#####
# Basic
#
echo "=========================================="
echo "*** Basic ***"
echo "=========================================="

# Run the Tiny Web server
tiny_port=$(free_port)
echo "Starting tiny on ${tiny_port}"
cd ./tiny
./tiny ${tiny_port} > ${HOME_DIR}/${DEBUG_DIR}/tiny.log 2>&1 &
tiny_pid=$!
cd ${HOME_DIR}

# Wait for tiny to start in earnest
wait_for_port_use "${tiny_port}"

# Run the proxy
proxy_port=$(free_port)
echo "Starting proxy on ${proxy_port}"
./proxy ${proxy_port} > ${DEBUG_DIR}/proxy.log 2>&1 &
proxy_pid=$!

# Wait for the proxy to start in earnest
wait_for_port_use "${proxy_port}"

echo ""
echo "Tiny URL: http://localhost:${tiny_port}"
echo "Proxy URL: http://localhost:${proxy_port}"
echo ""
echo "Tiny log: ${DEBUG_DIR}/tiny.log"
echo "Proxy log: ${DEBUG_DIR}/proxy.log"
echo ""

# Now do the test by fetching some text and binary files directly from
# Tiny and via the proxy, and then comparing the results.
numRun=0
numSucceeded=0
for file in ${BASIC_LIST}
do
    numRun=`expr $numRun + 1`
    echo "=========================================="
    echo "${numRun}: ${file}"
    echo "=========================================="
    clear_dirs

    # Fetch using the proxy
    echo "   Fetching ./tiny/${file} into ${PROXY_DIR} using the proxy"
    download_proxy $PROXY_DIR ${file} "http://localhost:${tiny_port}/${file}" "http://localhost:${proxy_port}"
    proxy_exit=$?

    # Fetch directly from Tiny
    echo "   Fetching ./tiny/${file} into ${NOPROXY_DIR} directly from Tiny"
    download_noproxy $NOPROXY_DIR ${file} "http://localhost:${tiny_port}/${file}"
    direct_exit=$?

    # Save files before comparing
    save_test_files "basic_test_${numRun}_${file}" "${file}"

    # Show file info
    show_file_info "${PROXY_DIR}/${file}" "${NOPROXY_DIR}/${file}"

    # Compare the two files
    echo "   Comparing the two files"

    # Check if both files exist
    if [ ! -f "${PROXY_DIR}/${file}" ]; then
        echo "   ✗ FAIL: Proxy file does not exist!"
        continue
    fi

    if [ ! -f "${NOPROXY_DIR}/${file}" ]; then
        echo "   ✗ FAIL: Direct file does not exist!"
        continue
    fi

    diff -q ${PROXY_DIR}/${file} ${NOPROXY_DIR}/${file} &> /dev/null
    if [ $? -eq 0 ]; then
        numSucceeded=`expr ${numSucceeded} + 1`
        echo "   ✓ SUCCESS: Files are identical."
    else
        echo "   ✗ FAILURE: Files differ."
        show_diff "${PROXY_DIR}/${file}" "${NOPROXY_DIR}/${file}" "${file}"
    fi
    echo ""
done

echo "Killing tiny and proxy"
kill $tiny_pid 2> /dev/null
wait $tiny_pid 2> /dev/null
kill $proxy_pid 2> /dev/null
wait $proxy_pid 2> /dev/null

basicScore=`expr ${MAX_BASIC} \* ${numSucceeded} / ${numRun}`

echo "=========================================="
echo "basicScore: $basicScore/${MAX_BASIC}"
echo "Passed: ${numSucceeded}/${numRun} tests"
echo "=========================================="

######
# Concurrency
#

echo ""
echo "=========================================="
echo "*** Concurrency ***"
echo "=========================================="

# Run the Tiny Web server
tiny_port=$(free_port)
echo "Starting tiny on port ${tiny_port}"
cd ./tiny
./tiny ${tiny_port} >> ${HOME_DIR}/${DEBUG_DIR}/tiny.log 2>&1 &
tiny_pid=$!
cd ${HOME_DIR}

# Wait for tiny to start in earnest
wait_for_port_use "${tiny_port}"

# Run the proxy
proxy_port=$(free_port)
echo "Starting proxy on port ${proxy_port}"
./proxy ${proxy_port} >> ${DEBUG_DIR}/proxy.log 2>&1 &
proxy_pid=$!

# Wait for the proxy to start in earnest
wait_for_port_use "${proxy_port}"

# Run a special blocking nop-server that never responds to requests
nop_port=$(free_port)
echo "Starting the blocking NOP server on port ${nop_port}"
./nop-server.py ${nop_port} > ${DEBUG_DIR}/nop-server.log 2>&1 &
nop_pid=$!

# Wait for the nop server to start in earnest
wait_for_port_use "${nop_port}"

echo ""
echo "Tiny URL: http://localhost:${tiny_port}"
echo "Proxy URL: http://localhost:${proxy_port}"
echo "NOP URL: http://localhost:${nop_port}"
echo ""

# Try to fetch a file from the blocking nop-server using the proxy
clear_dirs
echo "Trying to fetch a file from the blocking nop-server (will timeout)..."
download_proxy $PROXY_DIR "nop-file.txt" "http://localhost:${nop_port}/nop-file.txt" "http://localhost:${proxy_port}" &

sleep 1  # Give the blocking request a moment to start

# Fetch directly from Tiny
echo "Fetching ./tiny/${FETCH_FILE} into ${NOPROXY_DIR} directly from Tiny"
download_noproxy $NOPROXY_DIR ${FETCH_FILE} "http://localhost:${tiny_port}/${FETCH_FILE}"

# Fetch using the proxy
echo "Fetching ./tiny/${FETCH_FILE} into ${PROXY_DIR} using the proxy"
download_proxy $PROXY_DIR ${FETCH_FILE} "http://localhost:${tiny_port}/${FETCH_FILE}" "http://localhost:${proxy_port}"

# Save files
save_test_files "concurrency_test" "${FETCH_FILE}"

# Show file info
show_file_info "${PROXY_DIR}/${FETCH_FILE}" "${NOPROXY_DIR}/${FETCH_FILE}"

# See if the proxy fetch succeeded
echo "Checking whether the proxy fetch succeeded"
diff -q ${PROXY_DIR}/${FETCH_FILE} ${NOPROXY_DIR}/${FETCH_FILE} &> /dev/null
if [ $? -eq 0 ]; then
    concurrencyScore=${MAX_CONCURRENCY}
    echo "✓ SUCCESS: Was able to fetch tiny/${FETCH_FILE} from the proxy."
    echo "   (This means your proxy handles concurrent connections!)"
else
    concurrencyScore=0
    echo "✗ FAILURE: Was not able to fetch tiny/${FETCH_FILE} from the proxy."
    echo "   (This means your proxy is blocking on the NOP server)"
    show_diff "${PROXY_DIR}/${FETCH_FILE}" "${NOPROXY_DIR}/${FETCH_FILE}" "${FETCH_FILE}"
fi

# Clean up
echo ""
echo "Killing tiny, proxy, and nop-server"
kill $tiny_pid 2> /dev/null
wait $tiny_pid 2> /dev/null
kill $proxy_pid 2> /dev/null
wait $proxy_pid 2> /dev/null
kill $nop_pid 2> /dev/null
wait $nop_pid 2> /dev/null

echo "=========================================="
echo "concurrencyScore: $concurrencyScore/${MAX_CONCURRENCY}"
echo "=========================================="

#####
# Caching
#
echo ""
echo "=========================================="
echo "*** Cache ***"
echo "=========================================="

# Run the Tiny Web server
tiny_port=$(free_port)
echo "Starting tiny on port ${tiny_port}"
cd ./tiny
./tiny ${tiny_port} >> ${HOME_DIR}/${DEBUG_DIR}/tiny.log 2>&1 &
tiny_pid=$!
cd ${HOME_DIR}

# Wait for tiny to start in earnest
wait_for_port_use "${tiny_port}"

# Run the proxy
proxy_port=$(free_port)
echo "Starting proxy on port ${proxy_port}"
./proxy ${proxy_port} >> ${DEBUG_DIR}/proxy.log 2>&1 &
proxy_pid=$!

# Wait for the proxy to start in earnest
wait_for_port_use "${proxy_port}"

echo ""
echo "Tiny URL: http://localhost:${tiny_port}"
echo "Proxy URL: http://localhost:${proxy_port}"
echo ""

# Fetch some files from tiny using the proxy
clear_dirs
for file in ${CACHE_LIST}
do
    echo "Fetching ./tiny/${file} into ${PROXY_DIR} using the proxy"
    download_proxy $PROXY_DIR ${file} "http://localhost:${tiny_port}/${file}" "http://localhost:${proxy_port}"
    save_test_files "cache_warmup_${file}" "${file}"
done

# Kill Tiny
echo ""
echo "Killing tiny (cache test - server should be down now)"
kill $tiny_pid 2> /dev/null
wait $tiny_pid 2> /dev/null

sleep 1  # Give it a moment to die

# Now try to fetch a cached copy of one of the fetched files.
echo "Fetching a cached copy of ./tiny/${FETCH_FILE} into ${NOPROXY_DIR}"
echo "(Server is down, so this must come from cache)"
download_proxy $NOPROXY_DIR ${FETCH_FILE} "http://localhost:${tiny_port}/${FETCH_FILE}" "http://localhost:${proxy_port}"

# Save the cached fetch
save_test_files "cache_fetch" "${FETCH_FILE}"

# Show file info
show_file_info "${NOPROXY_DIR}/${FETCH_FILE}" "./tiny/${FETCH_FILE}"

# See if the proxy fetch succeeded by comparing it with the original
# file in the tiny directory
echo "Checking if cached file matches original"
diff -q ./tiny/${FETCH_FILE} ${NOPROXY_DIR}/${FETCH_FILE}  &> /dev/null
if [ $? -eq 0 ]; then
    cacheScore=${MAX_CACHE}
    echo "✓ SUCCESS: Was able to fetch tiny/${FETCH_FILE} from the cache."
else
    cacheScore=0
    echo "✗ FAILURE: Was not able to fetch tiny/${FETCH_FILE} from the proxy cache."
    if [ -f "${NOPROXY_DIR}/${FETCH_FILE}" ]; then
        show_diff "${NOPROXY_DIR}/${FETCH_FILE}" "./tiny/${FETCH_FILE}" "${FETCH_FILE}"
    else
        echo "   (Cached file does not exist)"
    fi
fi

# Kill the proxy
echo ""
echo "Killing proxy"
kill $proxy_pid 2> /dev/null
wait $proxy_pid 2> /dev/null

echo "=========================================="
echo "cacheScore: $cacheScore/${MAX_CACHE}"
echo "=========================================="

# Emit the total score
totalScore=`expr ${basicScore} + ${cacheScore} + ${concurrencyScore}`
maxScore=`expr ${MAX_BASIC} + ${MAX_CACHE} + ${MAX_CONCURRENCY}`
echo ""
echo "=========================================="
echo "           FINAL RESULTS"
echo "=========================================="
echo "Basic Tests:       $basicScore/${MAX_BASIC}"
echo "Concurrency Test:  $concurrencyScore/${MAX_CONCURRENCY}"
echo "Cache Test:        $cacheScore/${MAX_CACHE}"
echo "=========================================="
echo "Total Score:       ${totalScore}/${maxScore}"
echo "=========================================="
echo ""
echo "=========================================="
echo "           DEBUG INFORMATION"
echo "=========================================="
echo "All test files and logs saved to: ${DEBUG_DIR}/"
echo ""
echo "Contents:"
ls -la ${DEBUG_DIR}/ 2>/dev/null | tail -n +4
echo ""
echo "To view proxy output:"
echo "  cat ${DEBUG_DIR}/proxy.log"
echo ""
echo "To view tiny output:"
echo "  cat ${DEBUG_DIR}/tiny.log"
echo ""
echo "To view curl verbose output:"
echo "  cat ${DEBUG_DIR}/curl_verbose.log"
echo ""
echo "Test files are organized by test name in subdirectories"
echo "=========================================="
exit
