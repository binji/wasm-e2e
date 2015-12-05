#!/bin/bash
set -o nounset
set -o errexit

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
ROOT_DIR="$(dirname "${SCRIPT_DIR}")"
LLVM_DIR="${ROOT_DIR}/third_party/llvm"
CLANG_DIR="${LLVM_DIR}/tools/clang"
LLVM_SHA_FILE=${SCRIPT_DIR}/llvm.sha
CLANG_SHA_FILE=${SCRIPT_DIR}/clang.sha
LLVM_GIT_URL="http://llvm.org/git/"
LLVM_SVN_URL="https://llvm.org/svn/llvm-project/"
SVN_FILE="${LLVM_DIR}/.git/svn/.metadata"
DEPTH=5000
SYNC_TO_HEAD=NO
LLVM_USERNAME=NONE

while [[ $# > 0 ]]; do
  flag="$1"
  case $flag in
    --head)
      SYNC_TO_HEAD=YES
      ;;
    --llvm-username)
      LLVM_USERNAME="$2"
      shift # Pass over $2 value.
      ;;
    -h|--help)
      echo "Create or update LLVM-related repositories."
      echo " --head                  Sync to origin/master"
      echo " --llvm-username=a@b.com LLVM project username for commits"
      exit 0
      ;;
    *)
      echo "unknown arg ${flag}"
      exit 1
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
  local git_url=${LLVM_GIT_URL}$2.git
  local svn_url=${LLVM_SVN_URL}$3/trunk

  if [[ -d ${dir} ]]; then
    # Try to extend the depth if needed.
    if [[ `git -C ${dir} log --pretty=oneline | wc -l` -lt ${DEPTH} ]]; then
      git -C ${dir} fetch origin --depth ${DEPTH}
    else
      git -C ${dir} fetch origin
    fi
    if [[ -e ${SVN_FILE} ]]; then
      # The git repo has SVN history, keep the tags in sync.
      pushd ${dir}
      git checkout master
      git svn rebase -l
      popd
    fi
  else
    git -C "$(dirname ${dir})" clone --depth ${DEPTH} $git_url
    if [[ "${LLVM_USERNAME}" != "NONE" ]]; then
      # Initialize the SVN history:
      # llvm.org/docs/GettingStarted.html#for-developers-to-work-with-git-svn
      pushd ${dir}
      git svn init $svn_url --username=${LLVM_USERNAME}
      git config svn-remote.svn.fetch :refs/remotes/origin/master
      git svn rebase -l
      popd
    fi
  fi
}

fetch_or_clone ${LLVM_DIR} llvm llvm
fetch_or_clone ${CLANG_DIR} clang cfe

git -C ${LLVM_DIR} checkout ${LLVM_SHA}
git -C ${CLANG_DIR} checkout ${CLANG_SHA}

# If running w/ --head, also update llvm.sha and clang.sha to the new revisions.
if [[ ${SYNC_TO_HEAD} = "YES" ]]; then
  git -C ${LLVM_DIR} rev-parse HEAD > ${LLVM_SHA_FILE}
  git -C ${CLANG_DIR} rev-parse HEAD > ${CLANG_SHA_FILE}
fi
