#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"

cmake -G Ninja "${ROOT_DIR}/third_party/binaryen"
ninja
