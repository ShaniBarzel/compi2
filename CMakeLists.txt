cmake_minimum_required(VERSION 3.10)
project(compi2)

set(CMAKE_CXX_STANDARD 11)
add_executable(compi2 main.cpp grammar.cpp hw2.h hw2.cpp grammar.h tokens.h lex.yy.c lexer.lex)
