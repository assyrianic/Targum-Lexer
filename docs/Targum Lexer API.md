# C interface

# Datatypes

## struct TargumTokenInfo

```c
struct TargumTokenInfo {
	size_t   start, end, line, col;
	uint32_t tag;
};
```

### start
offset, in the source code, where the token starts at.

### end
offset, in the source code, where the token ends at.

### line
line, in the source code, where the tokens is at.

### col
column, in the source code, where the tokens is at.

### tag
unsigned integer value of the token. (defined by the config file used by the lexer)


## struct TargumLexer

```c
struct TargumLexer {
	struct HarbolArray     tokens;
	struct HarbolString    filename, src;
	struct HarbolMap       *cfg;
	char                   *iter, *line_start;
	size_t                  line, index, err_count;
	struct TargumTokenInfo *curr_tok;
};
```

### tokens
dynamic array of tokens. Always ends with an EOF token when tokens are generated.

### filename
string of the filename that the lexer is currently lexing.

### src
entire source code in a string object.

### cfg
pointer to hash table representing the config file.

### iter
char pointer used to iterate the source code.

### line_start
char pointer to the start of a line in the source code.
mostly used to determine the column of a token.

### line
current line in the source code.

### index
current index used to iterate the `tokens` array.

### err_count
how many lexer errors have ocurred. 

### curr_tok
token pointer as pointed to with `TargumLexer::index`.



# Functions/Methods


## targum_token_info_get_token
```c
uint32_t targum_token_info_get_token(const struct TargumTokenInfo *tokinfo);
```

### Description
self explanatory.

### Parameters
* `tokinfo` - pointer to a constant token info object.

### Return Value
unsigned integer representing the token value.


## targum_token_info_get_len
```c
size_t targum_token_info_get_len(const struct TargumTokenInfo *tokinfo);
```

### Description
Gets the string length of the token.

### Parameters
* `tokinfo` - pointer to a constant token info object.

### Return Value
size_t integer of the length of the token's string lexeme.


## targum_lexer_new_from_buffer
```c
struct TargumLexer *targum_lexer_new_from_buffer(const char src[], struct HarbolLinkMap *cfg);
```

### Description
Sets up a lexer object with a C string value to tokenize.
`cfg` can be NULL or, to share the same config file, pass an existing one.

### Parameters
* `src` - C string value to lexically analyze.
* `cfg` - map config structure to share (useful when importing/including other files).

### Return Value
pointer to an allocated lexer object.


## targum_lexer_new_from_file
```c
struct TargumLexer *targum_lexer_new_from_file(const char filename[], struct HarbolLinkMap *cfg);
```

### Description
Sets up a lexer object with a file to tokenize.
`cfg` can be NULL or, to share the same config file, pass an existing one.

### Parameters
* `filename` - C string of file to open and lexically analyze.
* `cfg` - map config structure to share (useful when importing/including other files).

### Return Value
pointer to an allocated lexer object.


## targum_lexer_make_from_buffer
```c
struct TargumLexer targum_lexer_make_from_buffer(const char src[], struct HarbolLinkMap *cfg);
```

### Description
Sets up a lexer object with a C string value to tokenize.
`cfg` can be NULL or, to share the same config file, pass an existing one.

### Parameters
* `src` - C string value to lexically analyze.
* `cfg` - map config structure to share (useful when importing/including other files).

### Return Value
lexer object.


## targum_lexer_make_from_file
```c
struct TargumLexer targum_lexer_make_from_file(const char filename[], struct HarbolLinkMap *cfg, bool *result);
```

### Description
Sets up a lexer object with a file to tokenize.
`cfg` can be NULL or, to share the same config file, pass an existing one.

### Parameters
* `filename` - C string of file to open and lexically analyze.
* `cfg` - map config structure to share (useful when importing/including other files).
* `result` - pointer to bool to store the result is making was successful.

### Return Value
lexer object.


## targum_lexer_clear
```c
void targum_lexer_clear(struct TargumLexer *lexer, bool free_cfg);
```

### Description
clears out the memory of a lexer object. 

### Parameters
* `lexer` - pointer to lexer object to free up.
* `free_cfg` - bool to free the map config structure held by the lexer object, set to false if sharing the config pointer.

### Return Value
None.


## targum_lexer_flush_tokens
```c
void targum_lexer_flush_tokens(struct TargumLexer *lexer);
```

### Description
clears out the token array of a lexer object.

### Parameters
* `lexer` - pointer to lexer object to free up.

### Return Value
None.


## targum_lexer_free
```c
void targum_lexer_free(struct TargumLexer **lexer_ref, bool free_cfg);
```

### Description
clears out the memory of a lexer object. Pointed-to-pointer is set to NULL.

### Parameters
* `lexer_ref` - pointer to a pointer to lexer object to free up.
* `free_cfg` - bool to free the map config structure held by the lexer object, set to false if sharing the config pointer.

### Return Value
None.


## targum_lexer_load_cfg_file
```c
bool targum_lexer_load_cfg_file(struct TargumLexer *lexer, const char cfg_file[]);
```

### Description
loads a config file to a lexer object.

### Parameters
* `lexer` - pointer to lexer object.
* `cfg_file` - config file name to load.

### Return Value
true if successful, false otherwise.


## targum_lexer_load_cfg_cstr
```c
bool targum_lexer_load_cfg_cstr(struct TargumLexer *lexer, const char cfg_cstr[]);
```

### Description
parse a manually constructed config C string to a lexer object.

### Parameters
* `lexer` - pointer to lexer object.
* `cfg_cstr` - C string of a config.

### Return Value
true if successful, false otherwise.


## targum_lexer_get_cfg
```c
struct HarbolLinkMap *targum_lexer_get_cfg(const struct TargumLexer *lexer);
```

### Description
gets the map structure that represents a config.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
true if successful, false otherwise.


## targum_lexer_get_err_count
```c
size_t targum_lexer_get_err_count(const struct TargumLexer *lexer);
```

### Description
gets the number of errors given from the lexer.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
amount of errors given.


## targum_lexer_get_filename
```c
const char *targum_lexer_get_filename(const struct TargumLexer *lexer);
```

### Description
retrieves C string of the filename the lexer is tokenizing.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
char pointer to the filename string.


## targum_lexer_get_token_index
```c
size_t targum_lexer_get_token_index(const struct TargumLexer *lexer);
```

### Description
self explanatory.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
unsigned integer of the current token index.


## targum_lexer_get_token_count
```c
size_t targum_lexer_get_token_count(const struct TargumLexer *lexer);
```

### Description
self explanatory.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
unsigned integer of how many tokens were tokenized.


## targum_lexer_get_token
```c
struct TargumTokenInfo *targum_lexer_get_token(const struct TargumLexer *lexer);
```

### Description
self explanatory.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
pointer to the current token managed by the lexer object.


## targum_lexer_peek_token
```c
TARGUM_API struct TargumTokenInfo *targum_lexer_peek_token(const struct TargumLexer *lexer, size_t lookahead);
```

### Description
retrieves tokens that are past the current token index.

### Parameters
* `lexer` - pointer to constant lexer object.
* `lookahead` - integer to add with the current token index.

### Return Value
pointer to the token from lookahead by the lexer, ending token if result exceeded the token array length.


## targum_lexer_advance
```c
struct TargumTokenInfo *targum_lexer_advance(struct TargumLexer *lexer, bool flush_tokens);
```

### Description
sets the current token managed by the lexer object, increment the tokens index, and returns the current token.

### Parameters
* `lexer` - pointer to lexer object.
* `flush_tokens` - bool to clear out token array (if tokenizing limited amount of tokens at a time).

### Return Value
pointer to the current token managed by the lexer object.


## targum_lexer_reset
```c
void targum_lexer_reset(struct TargumLexer *lexer);
```

### Description
Resets the lexer object's state and clears out all the tokens.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
None.


## targum_lexer_reset_token_index
```c
void targum_lexer_reset_token_index(struct TargumLexer *lexer);
```

### Description
Resets the token index back to 0 and sets the current token managed by the lexer to the first token.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
None.


## targum_lexer_generate_tokens
```c
bool targum_lexer_generate_tokens(struct TargumLexer *lexer);
```

### Description
Tokenizes the source file given to the lexer object.
Do not forget to load/supply a source file/string to tokenize.
Do not forget to load/supply a config file/string to use for tokenizing.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
true if successful, false otherwise.


## targum_lexer_remove_token
```c
bool targum_lexer_remove_token(struct TargumLexer *lexer, uint32_t tag);
```

### Description
Removes a single, specific token type from the dynamic token array held by the lexer object.

### Parameters
* `lexer` - pointer to lexer object.
* `tag` - unsigned integer token value to purge from the token array.

### Return Value
true if successful, false otherwise.


## targum_lexer_remove_token_type
```c
bool targum_lexer_remove_token_type(struct TargumLexer *lexer, uint32_t tag);
```

### Description
Purges a specific token type from the dynamic token array held by the lexer object.

### Parameters
* `lexer` - pointer to lexer object.
* `tag` - unsigned integer token value to purge from the token array.

### Return Value
true if successful, false otherwise.


## targum_lexer_remove_comments
```c
bool targum_lexer_remove_comments(struct TargumLexer *lexer);
```

### Description
Purges all comment tokens from the token array.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
true if successful, false otherwise.


## targum_lexer_remove_whitespace
```c
bool targum_lexer_remove_whitespace(struct TargumLexer *lexer);
```

### Description
Purges all whitespace tokens from the token array.
Only necessary if you set up your language config to tokenize (specific) whitespace like what Python does.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
true if successful, false otherwise.


## targum_lexer_purge_line
```c
bool targum_lexer_purge_line(struct TargumLexer *lexer, size_t line);
```

### Description
Purges all tokens from a specific line in the source code.

### Parameters
* `lexer` - pointer to lexer object.
* `line` - line in the source code.

### Return Value
true if purged at least one token from the line, false otherwise.


## targum_lexer_get_lexeme
```c
char *targum_lexer_get_lexeme(const struct TargumLexer *lexer, const struct TargumTokenInfo *tokinfo);
```

### Description
Gets the string lexeme from the lexer using token data.

### Parameters
* `lexer` - pointer to lexer object.
* `tokinfo` - pointer to token object.

### Return Value
returns string data directly from the token, NULL if source text was never loaded.
