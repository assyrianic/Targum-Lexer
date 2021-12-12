Dir
|--src
|----harbol_common_defines.h  => OS specific, Compiler specific, Standard specific defines for C.
|----harbol_common_includes.h => inline function helpers and standard types.
|----targum_lexer.(c/h)       => custom lexical analyzer, dependency on the data structures below.
|----test_driver.c            => targum lexer test driver program.
|----test.txt                 => test driver input file for messing around/testing.
|----tokens.cfg               => example config file.
|----Makefile                 => library makefile.
|------cfg                    => custom, JSON-like config parser. Dependency with 'map' & 'variant' libraries.
|------map                    => ordered hash table. Dependency with 'array' library.
|------str                    => immutable C string object type.
|------array                  => dynamic array.
|------variant                => variant type for holding any type of object.
|------lex                    => lexing tools module.
|------msg_sys                => logging system.
|--docs                       => folder you're in right now.
