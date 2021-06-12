Dir
|--src
|----harbol_common_defines.h  => OS specific, Compiler specific, Standard specific defines for C.
|----harbol_common_includes.h => inline function helpers and standard types.
|----targum_lexer.(c/h)       => custom lexical analyzer, dependency on the data structures below.
|----test_driver.c            => targum lexer test driver program.
|----tokens.cfg               => example config file.
|----Makefile                 => library makefile.
|------cfg                    => custom, JSON-like config parser. Dependency with map & variant.
|------map                    => ordered hash table. Dependency with array.
|------str                    => immutable C string object type.
|------array                  => dynamic array.
|------variant                => variant type for holding any type of object.
|------lex                    => lexing tools module.
|------msg_sys                => logging system.
|--docs                       => folder you're in right now.
