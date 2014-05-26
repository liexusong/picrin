# Picrin [![Build Status](https://travis-ci.org/wasabiz/picrin.png)](https://travis-ci.org/wasabiz/picrin)

Picrin is a lightweight scheme implementation intended to comply with full R7RS specification. Its code is written in pure C99 and does not requires any special external libraries installed on the platform.

## Features

- R7RS compatibility (but partial support)
- reentrant design (all VM states are stored in single global state object)
- bytecode interpreter (based on stack VM)
- direct threaded VM
- internal representation by nan-boxing
- conservative call/cc implementation (users can freely interleave native stack with VM stack)
- exact GC (simple mark and sweep, partially reference count is used as well)
- string representation by rope data structure
- support full set hygienic macro transformers, including implicit renaming macros
- extended library syntax
- advanced REPL support (multi-line input, etc)
- tiny & portable library (all functions will be in `libpicrin.so`)

## Documentation

See http://picrin.readthedocs.org/

## Homepage

Currently picrin is hosted on Github. You can freely send a bug report or pull-request, and fork the repository.

https://github.com/wasabiz/picrin

## IRC

There is a chat room on chat.freenode.org, channel #picrin.

## How to use it

- make `Makefile`

	Change directory to `build` then run `cmake` to create Makefile. Once `Makefile` is generated you can run `make` command to build picrin.

		$ cd build
        $ cmake ..

	Actually you don't necessarily need to move to `build` directory before running `cmake` (in that case `$ cmake .`), but I strongly recommend to follow above instruction.
    
- build

 	A built executable binary will be under bin/ directory and shared libraries under lib/.

        $ make

	If you are building picrin on other systems than x86_64, PIC_NAN_BOXING flag is automatically turned on (see include/picrin/config.h for detail).

- install

	Just running `make install`, picrin library, headers, and runtime binary are install on your system, by default into `/usr/local` directory. You can change this value via ccmake.

		$ make install

- run

	Before installing picrin, you can try picrin without breaking any of your system. Simply directly run the binary `bin/picrin` from terminal, or you can use `make` to execute it like this.

		$ make run

- debug run

	If you execute `cmake` with debug flag `-DCMAKE_BUILD_TYPE=Debug`, it builds the binary with all debug flags enabled (PIC_GC_STRESS, VM_DEBUG, DEBUG).

		$ cmake -DCMAKE_BUILD_TYPE=Debug ..
	

## Requirement

Picrin scheme depends on some external libraries to build the binary:

- perl
- lex (preferably, flex)
- getopt
- libedit (optional)
- regex.h of POSIX.1 (optional)

Optional libraries are, if cmake detected them, automatically enabled.
The compilation is tested only on Mac OSX and Ubuntu. I think (or hope) it'll be ok to compile and run on other operating systems such as Arch or Windows, but I don't guarantee :(

## Authors

See `AUTHORS`
