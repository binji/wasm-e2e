#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"
LLVM_DIR="${ROOT_DIR}/third_party/llvm"
CLANG_DIR="${LLVM_DIR}/tools/clang"

if [[ -d ${LLVM_DIR} ]]; then
  pushd ${LLVM_DIR}
  git pull origin
  pushd ${CLANG_DIR}
  git pull origin
  popd
  popd
else
  (cd "$(dirname ${LLVM_DIR})" && git clone --depth 1 http://llvm.org/git/llvm.git)
  (cd "$(dirname ${CLANG_DIR})" && git clone --depth 1 http://llvm.org/git/clang.git)
fi
