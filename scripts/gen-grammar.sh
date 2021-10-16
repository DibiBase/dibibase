#!/bin/sh

antlr4 -Dlanguage=Cpp -DexportMacro=DIBIBASE_PUBLIC -listener -visitor -lib grammars/ebnf -o include/lang/cql3 -package dibibase::lang::cql3 -Xexact-output-dir grammars/ebnf/CqlLexer.g4 grammars/ebnf/CqlParser.g4
