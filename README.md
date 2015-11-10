[![Build Status](https://travis-ci.org/WebAssembly/wasm-e2e.svg?branch=master)](https://travis-ci.org/WebAssembly/wasm-e2e)

# wasm-e2e

This repository contains tools and scripts to compile C/C++ to a WebAssembly
binary format, and run it.

Currently this includes:
- LLVM: for compiling C source to the ".s" format, which is the current
  output of WebAssembly by the LLVM backend.
- wasmate: for converting ".s" to the S-expression format as defined in the
  [spec repo](https://github.com/WebAssembly/spec).
- sexpr-wasm: for converting the S-expression format to the v8-native binary
  format.
- v8-native: for running the binary.

At this time, the contents of this repository are under development and known
to be incomplet and inkorrect.

Participation is welcome, though many of the changes will need to be landed in
other repositories.

## Cloning/Updating

Clone the repo, update the submodules, and update LLVM:

```
$ git clone https://github.com/WebAssembly/wasm-e2e
$ git submodule update --init
$ scripts/update-llvm.sh
```

LLVM is not included as a submodule because it is quite large and requires a
repo to be installed inside another repo. The `update-llvm.sh` script clones
with `--depth 1` to reduce the download size.

Updating the repo is almost the same:

```
$ git pull origin
$ git submodule update
$ scripts/update-llvm.sh
```

## Building

To build all the tools:

```
$ scripts/build-all.sh
```

You can also build each component separately:
- LLVM/Clang: `scripts/build-llvm.sh`
- v8-native/d8: `scripts/build-d8.sh`
- sexpr-wasm: `scripts/build-sexpr-wasm.sh`

## Running

Use the `script/run.py` Python script to compile and run a C source file:

```
$ scripts/run.py test/hello.c test/wasm.js
Hello, world!

$ scripts/run.py test/fizzbuzz.c test/wasm.js 
1
2
fizz
4
buzz
fizz
7
8
fizz
buzz
11
fizz
13
14
fizzbuzz
16
17
fizz
19
```

The script compiles the C source to v8-native binary format. The `wasm.js` file
is used as a wrapper for loading WebAssembly binary modules in d8.
