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
		printf("tokenization? '%s' | number of tokens:: %zu, token cap:: %zu, len of token:: %zu, bytes used: %zu\n", result ? "success!" : "failure!", lexer.tokens.len, lexer.tokens.cap, sizeof(struct TargumTokenInfo), lexer.tokens.cap * sizeof(struct TargumTokenInfo));
		const bool interactive = (argv[2] != NULL && !strcmp(argv[2], "--interactive")) || lexer.src.len > 6000; /// force interactive mode if massive file.
		size_t tokens_counted = 0;
		if( interactive ) {
			///*
			char controller = 0;
			while( controller != 'q' && controller != 'Q' ) {
				const struct TargumTokenInfo *const ti = targum_lexer_advance(&lexer, true);
				printf("token info:\n\tlexeme: '%.*s' | len: %u\n\ttoken value: '%u'\n\tpos:: start: '%u', end: '%u', line: '%u', col: '%u'\n\npress 'q' to stop, any other key to continue.", (int)(ti->end - ti->start), &lexer.src.cstr[ti->start], ti->end - ti->start, ti->tag, ti->start, ti->end, ti->line, ti->col);
				controller = getchar();
			}
			//*/
			/*
			struct TargumTokenInfo old_t = {0}; 
			size_t timer = 0;
			while( targum_lexer_generate_tokens(&lexer) ) {
				const struct TargumTokenInfo *const ti = harbol_array_get(&lexer.tokens, 976308u - 2, sizeof *ti);
				printf("2nd to last token:: tag, line, col: %u, %u, %u - '%.*s'\n", ti->tag, ti->line, ti->col, (int)(ti->end - ti->start), &lexer.src.cstr[ti->start]);
				if( ti->tag==0 || (!memcmp(&old_t, ti, sizeof *ti) && timer > 100) ) {
					break;
				}
				tokens_counted += lexer.tokens.len;
				old_t = *ti;
				timer++;
				targum_lexer_flush_tokens(&lexer);
			}
			printf("final tokens counted: %zu | tag, line, col: %u, %u, %u\n", tokens_counted, old_t.tag, old_t.line, old_t.col);
			*/
		} else {
			FILE *restrict print_text = fopen("targum_lexer_tokens.txt", "w");
			assert( print_text != NULL && "failed to create targum_lexer_tokens.txt!" );
			for( const struct TargumTokenInfo *ti = targum_lexer_advance(&lexer, true); ti != NULL && ti->tag != 0; ti = targum_lexer_advance(&lexer, true) ) {
				fprintf(print_text, "token info:\n\tlexeme: '%.*s' | len: %u\n\ttoken value: '%u'\n\tpos:: start: '%u', end: '%u', line: '%u', col: '%u'\n\n", (int)(ti->end - ti->start), &lexer.src.cstr[ti->start], ti->end - ti->start, ti->tag, ti->start, ti->end, ti->line, ti->col);
			}
			fclose(print_text); print_text = NULL;
		}
		targum_lexer_clear(&lexer, true);
	}
}
