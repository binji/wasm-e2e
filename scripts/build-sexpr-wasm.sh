#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"

make -C "${ROOT_DIR}/third_party/sexpr-wasm-prototype"
