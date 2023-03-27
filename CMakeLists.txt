cmake_minimum_required(VERSION 3.24)
project(proj631_huffman)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(.idea)

add_executable(proj631_huffman
        .idea/.gitignore
        .idea/modules.xml
        .idea/proj631_huffman.iml
        .idea/vcs.xml
        .idea/workspace.xml
        .gitignore
        decompression.c
        exemple_comp.bin
        exemple_freq.txt
        output.txt
        )
