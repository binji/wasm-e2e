#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$0")"

${SCRIPT_DIR}/build-llvm.sh
${SCRIPT_DIR}/build-d8.sh
${SCRIPT_DIR}/build-sexpr-wasm.sh
