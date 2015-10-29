#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"
LLVM_DIR="${ROOT_DIR}/third_party/llvm"
CLANG_DIR="${LLVM_DIR}/tools/clang"
LLVM_SHA_FILE=${SCRIPT_DIR}/llvm.sha
CLANG_SHA_FILE=${SCRIPT_DIR}/clang.sha
DEPTH=5000
SYNC_TO_HEAD=NO

while [[ $# > 0 ]]; do
  flag="$1"
  case $flag in
    --head)
      SYNC_TO_HEAD=YES
      ;;
    *)
      echo "unknown arg ${flag}"
      ;;
  esac
  shift
done

if [[ ${SYNC_TO_HEAD} = "YES" ]]; then
  LLVM_SHA=origin/master
  CLANG_SHA=origin/master
else
  LLVM_SHA=`cat ${LLVM_SHA_FILE}`
  CLANG_SHA=`cat ${CLANG_SHA_FILE}`
fi

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

# If running w/ --head, also update llvm.sha and clang.sha to the new revisions
if [[ ${SYNC_TO_HEAD} = "YES" ]]; then
  git -C ${LLVM_DIR} rev-parse HEAD > ${LLVM_SHA_FILE}
  git -C ${CLANG_DIR} rev-parse HEAD > ${CLANG_SHA_FILE}
fi
