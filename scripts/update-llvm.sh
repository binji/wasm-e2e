#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"
LLVM_DIR="${ROOT_DIR}/third_party/llvm"
CLANG_DIR="${LLVM_DIR}/tools/clang"
DEPTH=5000

LLVM_SHA=644d8eaae43035481913aa898626305c59343f85
CLANG_SHA=e90410ed3ac353b1bbc98970b2954ffeff65ab16

fetch_or_clone() {
  local dir=$1
  local url=$2

  if [[ -d ${dir} ]]; then
    # Try to extend the depth if needed.
    if [[ `git -C ${dir} log --pretty=oneline | wc -l` -lt ${DEPTH} ]]; then
      git -C ${dir} fetch origin --depth ${DEPTH}
    else
      git -C ${dir} fetch origin
    fi
  else
    git -C "$(dirname ${dir})" clone --depth ${DEPTH} $url
  fi
}

fetch_or_clone ${LLVM_DIR} http://llvm.org/git/llvm.git
fetch_or_clone ${CLANG_DIR} http://llvm.org/git/clang.git

git -C ${LLVM_DIR} checkout ${LLVM_SHA}
git -C ${CLANG_DIR} checkout ${CLANG_SHA}
