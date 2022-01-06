#!/bin/bash
./packcc.elf -d lee_parser.peg > build/packcc.log && (
	pushd build
	clang -g -c ../*.c &&
	clang -g -o ../lee.elf *.o
	popd
)
