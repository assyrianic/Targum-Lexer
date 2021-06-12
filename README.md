# Targum Lexer

## Introduction
Part of the Targum Compiler Frontend Suite, the Targum Lexer is a highly configurable C/Golang-like lexer!


### Features

* Tokenizes single line and multi line comments.
* C and Golang style numbers, strings, character literals aka runes, and identifiers.
* Completely configurable by a config file!
* Perfect for lexing a custom C-like or Golang-like language!


## Usage

```c
#include "targum_lexer.h"

int main(const int argc, char *argv[static 1])
{
	if( argv[1]==NULL )
		return -1;
	
	struct TargumLexer lexer = targum_lexer_create_from_file(argv[1], NULL);
	targum_lexer_load_cfg_file(&lexer, "tokens.cfg");
	targum_lexer_generate_tokens(&lexer);
	targum_lexer_clear(&lexer, true);
}
```

## Contributing

To submit a patch, first file an issue and/or present a pull request.

## Help

If you need help or have any question, make an issue on the github repository.
Simply drop a message or your question and you'll be reached in no time!

## Installation

### Requirements

C99 compliant compiler and libc implementation with stdlib.h, stdio.h, and stddef.h.

### Installation

To build the library, simply run `make` which will make the static library version of libtargum_lexer.

To clean up the `.o` files, run `make clean`.

To build a debug version of the library, run `make debug`.

### Testing

For testing code changes or additions, simply run `make test` with `test_driver.c` in the directory which will build an executable called `test_driver`.


## Credits

* Kevin Yonan - Lead Developer of the Targum Compiler Frontend Suite.


## License

This project is licensed under Apache License.
