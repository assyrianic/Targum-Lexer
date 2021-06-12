#include <assert.h>
#include "targum_lexer.h"

int main(const int argc, char *restrict argv[restrict static 1])
{
	if( argc<2 ) {
		puts("Targum Lexer Driver Error: missing text file.");
		return -1;
	} else if( !strcmp(argv[1], "--help") ) {
		puts("Targum Lexer Driver - 'test_driver filename'\nExample: 'test_driver file.ext'");
	} else if( !strcmp(argv[1], "--version") ) {
		puts("Targum Lexer Driver Version " TARGUM_LEXER_VERSION_STRING);
	} else {
		struct TargumLexer lexer = targum_lexer_make_from_file(argv[1], NULL, &( bool ){false});
		assert( targum_lexer_load_cfg_file(&lexer, "tokens.cfg") && "failed to load tokens.cfg!" );
		const bool result = targum_lexer_generate_tokens(&lexer);
		printf("tokenization? '%s'\n", result ? "success!" : "failure!");
		const bool interactive = (argv[2] != NULL && !strcmp(argv[2], "--interactive")) || lexer.src.len > 6000; /// force interactive mode if massive file.
		if( interactive ) {
			char controller = 0;
			while( controller != 'q' && controller != 'Q' ) {
				const struct TargumTokenInfo *const ti = targum_lexer_advance(&lexer, true);
				printf("token info:\n\tlexeme: '%s' | len: %zu\n\tfilename: '%s'\n\ttoken value: '%u'\n\tpos:: start: '%zu', end: '%zu', line: '%zu', col: '%zu'\n\npress 'q' to stop, any other key to continue.", ti->lexeme.cstr, ti->lexeme.len, ti->filename->cstr, ti->tag, ti->start, ti->end, ti->line, ti->col);
				controller = getchar();
			}
		} else {
			FILE *restrict print_text = fopen("targum_lexer_tokens.txt", "w");
			assert( print_text != NULL && "failed to create targum_lexer_tokens.txt!" );
			for( const struct TargumTokenInfo *ti = targum_lexer_advance(&lexer, true); ti != NULL && ti->tag != 0; ti = targum_lexer_advance(&lexer, true) ) {
				fprintf(print_text, "token info:\n\tlexeme: '%s' | len: %zu\n\tfilename: '%s'\n\ttoken value: '%u'\n\tpos:: start: '%zu', end: '%zu', line: '%zu', col: '%zu'\n\n", ti->lexeme.cstr, ti->lexeme.len, ti->filename->cstr, ti->tag, ti->start, ti->end, ti->line, ti->col);
			}
			fclose(print_text); print_text = NULL;
		}
		targum_lexer_clear(&lexer, true);
	}
}