#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"
LLVM_DIR="${ROOT_DIR}/third_party/llvm"
BUILD_DIR="${LLVM_DIR}/build"
CMAKE_FLAGS="\
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DLLVM_BUILD_TESTS=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly \
  -DLLVM_TARGETS_TO_BUILD=X86"

if [[ ! -d ${BUILD_DIR} ]]; then
  mkdir -p ${BUILD_DIR}
  (cd ${BUILD_DIR} && cmake -G Ninja .. ${CMAKE_FLAGS})
fi

pushd ${BUILD_DIR}
time ninja bin/clang
popd
