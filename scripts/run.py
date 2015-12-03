#!/usr/bin/env python

import argparse
import os
import os.path
import shutil
import subprocess
import sys
import tempfile

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT_DIR = os.path.dirname(SCRIPT_DIR)
BIN_DIR = os.path.join(REPO_ROOT_DIR, 'bin')

WACC = os.path.join(BIN_DIR, 'wacc')
WASMATE = os.path.join(BIN_DIR, 'wasmate.py')
SEXPR_WASM = os.path.join(BIN_DIR, 'sexpr-wasm')
D8 = os.path.join(BIN_DIR, 'd8')
WASM_JS = os.path.join(REPO_ROOT_DIR, 'test', 'wasm.js')


class Error(Exception):
  pass


def main(args):
  parser = argparse.ArgumentParser()
  parser.add_argument('c_file', help='test C file')
  parser.add_argument('js_file', nargs='?',
                      help='test JS file', default=WASM_JS)
  parser.add_argument('--dump', help='directory to store intermediate files')
  options = parser.parse_args(args)

  basename = os.path.splitext(os.path.basename(options.c_file))[0]
  if not options.dump:
    clean_temp_dir = True
    temp_dir = tempfile.mkdtemp(prefix='wasm-e2e-run-')
  else:
    clean_temp_dir = False
    temp_dir = options.dump
    if not os.path.exists(temp_dir):
      os.makedirs(temp_dir)
  s_file = os.path.join(temp_dir, basename + '.s')
  wast_file = os.path.join(temp_dir, basename + '.wast')
  wasm_file = os.path.join(temp_dir, basename + '.wasm')

  try:
    subprocess.check_call([WACC, '-fno-builtin', options.c_file, '-o', s_file])
    subprocess.check_call([sys.executable, WASMATE, s_file, '-o', wast_file])
    subprocess.check_call([SEXPR_WASM, wast_file, '-o', wasm_file])
    process = subprocess.Popen([D8, options.js_file, '--', wasm_file],
                               stderr=subprocess.PIPE)
    _, stderr = process.communicate()
    if process.returncode != 0:
      raise Error(stderr)
  finally:
    if clean_temp_dir:
      shutil.rmtree(temp_dir)


if __name__ == '__main__':
  try:
    sys.exit(main(sys.argv[1:]))
  except (Error, OSError, subprocess.CalledProcessError) as e:
    sys.stderr.write(str(e) + '\n')
    sys.exit(1)
