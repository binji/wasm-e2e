#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"

cd "${ROOT_DIR}/third_party/binaryen"
cmake -G Ninja
ninja
