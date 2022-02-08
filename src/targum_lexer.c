#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include "targum_lexer.h"

#ifdef OS_WINDOWS
#	define TARGUM_LIB
#endif


TARGUM_API void targum_token_info_clear(struct TargumTokenInfo *const tokinfo)
{
	harbol_string_clear(&tokinfo->lexeme);
}

TARGUM_API const char *targum_token_info_get_lexeme(const struct TargumTokenInfo *const tokinfo)
{
	return( tokinfo->lexeme.cstr==NULL )? "" : tokinfo->lexeme.cstr;
}

TARGUM_API const char *targum_token_info_get_filename(const struct TargumTokenInfo *const tokinfo)
{
	return( tokinfo->filename==NULL )? "user-defined file" : tokinfo->filename->cstr;
}

TARGUM_API uint32_t targum_token_info_get_token(const struct TargumTokenInfo *const tokinfo)
{
	return tokinfo->tag;
}

/*********************************************************************/

TARGUM_API struct TargumLexer *targum_lexer_new_from_buffer(const char src[static 1], struct HarbolMap *const restrict cfg)
{
	struct TargumLexer *restrict lexer = calloc(1, sizeof *lexer);
	if( lexer != NULL ) {
		*lexer = targum_lexer_make_from_buffer(src, cfg);
	}
	return lexer;
}

TARGUM_API struct TargumLexer *targum_lexer_new_from_file(const char filename[static 1], struct HarbolMap *const restrict cfg)
{
	struct TargumLexer *restrict lexer = calloc(1, sizeof *lexer);
	if( lexer != NULL ) {
		bool res = false;
		*lexer = targum_lexer_make_from_file(filename, cfg, &res);
		if( !res ) {
			free(lexer); lexer = NULL;
		}
	}
	return lexer;
}

static NEVER_NULL(1) void _setup_lexer(struct TargumLexer *const lexer, struct HarbolMap *const cfg)
{
	lexer->iter = lexer->line_start = lexer->src.cstr;
	lexer->cfg  = cfg;
	lexer->line = 1;
	harbol_array_init(&lexer->tokens, sizeof(struct TargumTokenInfo), ARRAY_DEFAULT_SIZE);
}

TARGUM_API struct TargumLexer targum_lexer_make_from_buffer(const char src[static 1], struct HarbolMap *const restrict cfg)
{
	struct TargumLexer lexer = {0};
	lexer.src = harbol_string_make(src, &( bool ){false});
	harbol_string_format(&lexer.filename, true, "%p", src);
	_setup_lexer(&lexer, cfg);
	return lexer;
}

TARGUM_API struct TargumLexer targum_lexer_make_from_file(const char filename[static 1], struct HarbolMap *const restrict cfg, bool *const restrict res)
{
	struct TargumLexer lexer = {0};
	FILE *restrict src_file = fopen(filename, "r");
	if( src_file==NULL ) {
		*res = false;
		return lexer;
	}
	
	*res = harbol_string_read_from_file(&lexer.src, src_file);
	fclose(src_file); src_file = NULL;
	if( !*res ) {
		harbol_string_clear(&lexer.src);
	} else {
		lexer.filename = harbol_string_make(filename, &( bool ){false});
		_setup_lexer(&lexer, cfg);
	}
	return lexer;
}

TARGUM_API void targum_lexer_clear(struct TargumLexer *const lexer, const bool free_config)
{
	targum_lexer_flush_tokens(lexer);
	if( free_config ) {
		harbol_cfg_free(&lexer->cfg);
	}
	harbol_string_clear(&lexer->filename);
	harbol_string_clear(&lexer->src);
}


TARGUM_API void targum_lexer_flush_tokens(struct TargumLexer *const lexer)
{
	for( size_t i=0; i < lexer->tokens.len; i++ ) {
		struct TargumTokenInfo *const t = harbol_array_get(&lexer->tokens, i, sizeof *t);
		harbol_string_clear(&t->lexeme);
	}
	harbol_array_clear(&lexer->tokens);
	lexer->index = 0;
	lexer->curr_tok = NULL;
}

TARGUM_API void targum_lexer_free(struct TargumLexer **const lexer_ref, const bool free_config)
{
	targum_lexer_clear(*lexer_ref, free_config);
	free(*lexer_ref); *lexer_ref = NULL;
}

TARGUM_API bool targum_lexer_load_cfg_file(struct TargumLexer *const restrict lexer, const char filename[static 1])
{
	lexer->cfg = harbol_cfg_parse_file(filename);
	return lexer->cfg != NULL;
}

TARGUM_API bool targum_lexer_load_cfg_cstr(struct TargumLexer *const restrict lexer, const char cfg_cstr[static 1])
{
	lexer->cfg = harbol_cfg_parse_cstr(cfg_cstr);
	return lexer->cfg != NULL;
}

TARGUM_API struct HarbolMap *targum_lexer_get_cfg(const struct TargumLexer *const lexer)
{
	return lexer->cfg;
}

TARGUM_API const char *targum_lexer_get_filename(const struct TargumLexer *const lexer)
{
	return lexer->filename.cstr;
}

TARGUM_API size_t targum_lexer_get_token_index(const struct TargumLexer *const lexer)
{
	return lexer->index;
}

TARGUM_API size_t targum_lexer_get_token_count(const struct TargumLexer *const lexer)
{
	return lexer->tokens.len;
}

TARGUM_API struct TargumTokenInfo *targum_lexer_get_token(const struct TargumLexer *const lexer)
{
	return lexer->curr_tok;
}

TARGUM_API struct TargumTokenInfo *targum_lexer_peek_token(const struct TargumLexer *const lexer, const size_t lookahead)
{
	return harbol_array_get(&lexer->tokens, ( (lexer->index + lookahead) >= lexer->tokens.len )? lexer->tokens.len - 1 : lexer->index + lookahead, sizeof *lexer->curr_tok);
}

TARGUM_API struct TargumTokenInfo *targum_lexer_advance(struct TargumLexer *const lexer, const bool flush_old_tokens)
{
	if( lexer->cfg==NULL || lexer->tokens.len==0 ) {
		return lexer->curr_tok;
	}
	if( lexer->index < lexer->tokens.len ) {
		lexer->curr_tok = harbol_array_get(&lexer->tokens, lexer->index++, sizeof *lexer->curr_tok);
	} else {
		const intmax_t *const max_toks = harbol_cfg_get_int(lexer->cfg, "tokens.on demand");
		if( max_toks != NULL && *max_toks > 0 ) {
			if( flush_old_tokens ) {
				targum_lexer_flush_tokens(lexer);
			}
			targum_lexer_generate_tokens(lexer);
			lexer->curr_tok = harbol_array_get(&lexer->tokens, lexer->index++, sizeof *lexer->curr_tok);
		}
	}
	return lexer->curr_tok;
}

TARGUM_API void targum_lexer_reset(struct TargumLexer *lexer)
{
	targum_lexer_flush_tokens(lexer);
	_setup_lexer(lexer, lexer->cfg);
}

TARGUM_API void targum_lexer_reset_token_index(struct TargumLexer *lexer)
{
	lexer->index = 0;
	lexer->curr_tok = harbol_array_get(&lexer->tokens, lexer->index++, sizeof *lexer->curr_tok);
}

TARGUM_API bool targum_lexer_generate_tokens(struct TargumLexer *const lexer)
{
	bool result = false;
	/// Make sure essential data is pre-loaded.
	if( lexer->iter==NULL || harbol_string_empty(&lexer->src) ) {
		harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "critical error", NULL, NULL, "No source file loaded! Failed to generate tokens.");
		goto targum_lex_err_exit;
	} else if( lexer->cfg==NULL ) {
		harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "critical error", NULL, NULL, "No config loaded! Failed to generate tokens.");
		goto targum_lex_err_exit;
	}
	
	/// Make sure our config structure has what is required.
	struct HarbolMap *const tokens = harbol_cfg_get_section(lexer->cfg, "tokens");
	if( tokens==NULL ) {
		harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "critical error", NULL, NULL, "missing tokens section in config file/string! Failed to generate tokens.");
		goto targum_lex_err_exit;
	}
	
	struct HarbolMap
		*const whitespace = harbol_cfg_get_section(tokens, "whitespace"),
		*const comments   = harbol_cfg_get_section(tokens, "comments"),
		*const keywords   = harbol_cfg_get_section(tokens, "keywords"),
		*const operators  = harbol_cfg_get_section(tokens, "operators")
	;
	if( keywords==NULL && operators==NULL ) {
		harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "critical error", NULL, NULL, "Missing both keywords and operators sections in config! Either have a keyword or operator section. Failed to generate tokens.");
		goto targum_lex_err_exit;
	}
	
	/// Optional config settings, shoot warnings if the keys don't exist.
	bool golang_style = false;
	{
		const bool *const use_golang_style = harbol_cfg_get_bool(tokens, "use golang-style");
		if( use_golang_style != NULL ) {
			golang_style = *use_golang_style;
		} else {
			harbol_warn_msg(NULL, lexer->filename.cstr, "system warning", NULL, NULL, "Missing 'use golang-style' key, defaulting to C-style tokens.");
		}
	}
	
	size_t max_toks = 0;
	{
		const intmax_t *const demand_size = harbol_cfg_get_int(tokens, "on demand");
		if( demand_size != NULL ) {
			max_toks = ( size_t )(*demand_size);
		} else {
			harbol_warn_msg(NULL, lexer->filename.cstr, "system warning", NULL, NULL, "'on demand' key doesn't exist, defaulting to generating all tokens.");
		}
	}
	
	bool lex_comments = false;
	{
		const bool *const lex_comment_val = harbol_cfg_get_bool(tokens, "tokenize comments");
		if( lex_comment_val != NULL ) {
			lex_comments = *lex_comment_val;
		} else {
			harbol_warn_msg(NULL, lexer->filename.cstr, "system warning", NULL, NULL, "'tokenize comments' key doesn't exist, defaulting to ignoring comments.");
		}
	}
	
	{
		/// make sure the default tokens are validated.
		const char *names[] = {
			"invalid",
			"comment",
			"identifier",
			"integer",
			"float",
			"string",
			"rune",
		};
		for( const char **iter = &names[0]; iter < 1[&names]; iter++ ) {
			if( harbol_cfg_get_int(tokens, *iter)==NULL ) {
				harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "critical error", NULL, NULL, "Missing '%s' key in config file! Failed to generate tokens.", *iter);
				goto targum_lex_err_exit;
			}
		}
	}
	
	size_t token_count = 0;
	while( *lexer->iter != 0 ) {
		if( harbol_array_full(&lexer->tokens) && !harbol_array_grow(&lexer->tokens, sizeof(struct TargumTokenInfo)) ) {
			harbol_err_msg(NULL, lexer->filename.cstr, "memory error", NULL, NULL, "token vector failed to grow.");
			return false;
		}
		
		if( max_toks > 0 && token_count >= max_toks ) {
			return true;
		} else if( is_whitespace(*lexer->iter) ) {
			/// check white space if they're considered legit tokens.
			const int_fast8_t s = *lexer->iter++;
			if( s=='\n' ) {
				lexer->line++;
				lexer->line_start = lexer->iter;
			}
			
			if( whitespace != NULL ) {
				const char *whitespace_key = NULL;
				switch( s ) {
					case ' ' : whitespace_key = "space";   break;
					case '\t': whitespace_key = "tab";     break;
					case '\n': whitespace_key = "newline"; break;
				}
				const intmax_t *const token_value = harbol_cfg_get_int(whitespace, whitespace_key);
				if( token_value != NULL ) {
					struct TargumTokenInfo tok = {
						.start    = ( uintptr_t )(lexer->iter - lexer->src.cstr),
						.col      = ( uintptr_t )(lexer->iter - lexer->line_start),
						.line     = lexer->line,
						.filename = &lexer->filename,
						.tag      = *token_value
					};
					while( *lexer->iter==s ) {
						lexer->iter++;
						if( s=='\n' ) {
							lexer->line++;
							lexer->line_start = lexer->iter;
						}
					}
					tok.end = ( uintptr_t )(lexer->iter - lexer->src.cstr + 1);
					harbol_string_add_char(&tok.lexeme, s);
					harbol_array_insert(&lexer->tokens, &tok, sizeof tok);
					token_count++;
				}
			}
			continue;
		} else if( is_alphabetic(*lexer->iter) ) {
			/// check identifiers/keywords.
			struct TargumTokenInfo tok = {
				.start    = ( uintptr_t )(lexer->iter - lexer->src.cstr),
				.col      = ( uintptr_t )(lexer->iter - lexer->line_start),
				.line     = lexer->line,
				.filename = &lexer->filename,
			};
			
			while( *lexer->iter != 0 && is_possible_id(*lexer->iter) ) {
				harbol_string_add_char(&tok.lexeme, *lexer->iter++);
			}
			tok.end = ( uintptr_t )(lexer->iter - lexer->src.cstr);
			
			/// check if we got a keyword or identifier.
			const intmax_t *const keyword = harbol_cfg_get_int(keywords, tok.lexeme.cstr);
			const intmax_t *const id_val = harbol_cfg_get_int(tokens, "identifier");
			tok.tag = ( keyword != NULL )? *keyword : *id_val;
			harbol_array_insert(&lexer->tokens, &tok, sizeof tok);
			token_count++;
		} else if( is_decimal(*lexer->iter) || *lexer->iter=='.' ) {
			const bool dot = *lexer->iter=='.';
			/// Check number literal.
			struct TargumTokenInfo tok = {
				.start    = ( uintptr_t )(lexer->iter - lexer->src.cstr),
				.col      = ( uintptr_t )(lexer->iter - lexer->line_start),
				.line     = lexer->line,
				.filename = &lexer->filename
			};
			bool is_float = false;
			char *end = NULL;
			const int lex_result = ( golang_style? lex_go_style_number : lex_c_style_number )(lexer->iter, ( const char** )(&end), &tok.lexeme, &is_float);
			if( lex_result != HarbolLexNoErr && !dot ) {
				harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "error", &lexer->line, &( const size_t ){ ( uintptr_t )(lexer->iter - lexer->line_start) }, "invalid number: %s - '%s'", lex_get_err(lex_result), tok.lexeme.cstr);
				harbol_string_clear(&tok.lexeme);
				goto targum_lex_err_exit;
			} else if( lex_result != HarbolLexNoErr && dot ) {
				/// invalid number, jump to the operators section.
				harbol_string_clear(&tok.lexeme);
				goto check_operators;
			} else {
				if( end != NULL ) {
					lexer->iter = end;
				}
				tok.end = ( uintptr_t )(lexer->iter - lexer->src.cstr);
				tok.tag = ( uint32_t )(*harbol_cfg_get_int(tokens, is_float? "float" : "integer"));
				harbol_array_insert(&lexer->tokens, &tok, sizeof tok);
				token_count++;
			}
		} else {
		check_operators:;
			/// check operators and comments!
			if( comments != NULL ) {
				bool got_something = false;
				for( size_t i=0; i < comments->len; i++ ) {
					if( !strncmp(lexer->iter, ( const char* )(comments->keys[i]), comments->keylens[i]-1) ) {
						const struct HarbolString *const end_comment = harbol_cfg_get_str(comments, ( const char* )(comments->keys[i]));
						if( !lex_comments ) {
							lexer->iter = ( end_comment==NULL || end_comment->len==0 )? ( char* )(skip_single_line_comment(lexer->iter, &lexer->line)) : ( char* )(skip_multi_line_comment(lexer->iter, end_comment->cstr, end_comment->len, &lexer->line));
							got_something = true;
							break;
						}
						
						struct TargumTokenInfo tok = {
							.start    = ( uintptr_t )(lexer->iter - lexer->src.cstr),
							.col      = ( uintptr_t )(lexer->iter - lexer->line_start),
							.line     = lexer->line,
							.filename = &lexer->filename
						};
						tok.tag = *harbol_cfg_get_int(tokens, "comment");
						char *end = NULL;
						const bool result = ( end_comment==NULL || end_comment->len==0 )?
								lex_single_line_comment(lexer->iter, ( const char** )(&end), &tok.lexeme, &lexer->line)
								: lex_multi_line_comment(lexer->iter, ( const char** )(&end), end_comment->cstr, end_comment->len, &tok.lexeme, &lexer->line);
						
						if( !result ) {
							harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "error", &lexer->line, &( const size_t ){ ( uintptr_t )(lexer->iter - lexer->line_start) }, "invalid %s comment! - '%s'", ( end_comment==NULL || end_comment->len==0 )? "single-line" : "multi-line", tok.lexeme.cstr);
							harbol_string_clear(&tok.lexeme);
							goto targum_lex_err_exit;
						}
						if( end != NULL ) {
							lexer->iter = end;
						}
						tok.end = ( uintptr_t )(lexer->iter - lexer->src.cstr);
						harbol_array_insert(&lexer->tokens, &tok, sizeof tok);
						token_count++;
						got_something = true;
						break;
					}
				}
				if( got_something ) {
					continue;
				}
			}
			
			/// placing this code here so we don't glitch out "string-like" comments.
			if( *lexer->iter=='\'' || *lexer->iter=='"' || (golang_style && *lexer->iter=='`') ) {
				/// check strings!
				const int_fast8_t quote = *lexer->iter;
				struct TargumTokenInfo tok = {
					.start    = ( uintptr_t )(lexer->iter - lexer->src.cstr),
					.col      = ( uintptr_t )(lexer->iter - lexer->line_start),
					.line     = lexer->line,
					.filename = &lexer->filename
				};
				char *end = NULL;
				const int lex_result = ( golang_style? lex_go_style_str : lex_c_style_str )(lexer->iter, ( const char** )(&end), &tok.lexeme);
				if( lex_result > HarbolLexNoErr ) {
					harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "error", &lexer->line, &( const size_t ){ ( uintptr_t )(lexer->iter - lexer->line_start) }, (quote=='"' || (golang_style && quote=='`'))? "invalid string! %s - '%s'" : "invalid rune! %s - '%s'", lex_get_err(lex_result), tok.lexeme.cstr);
					harbol_string_clear(&tok.lexeme);
					goto targum_lex_err_exit;
				}
				if( end != NULL ) {
					lexer->iter = end;
				}
				tok.end = ( uintptr_t )(lexer->iter - lexer->src.cstr);
				tok.tag = (quote=='"' || (golang_style && quote=='`'))? *harbol_cfg_get_int(tokens, "string") : *harbol_cfg_get_int(tokens, "rune");
				harbol_array_insert(&lexer->tokens, &tok, sizeof tok);
				token_count++;
				continue;
			}
			
			if( operators != NULL ) {
				size_t operator_size = 0;
				size_t match = operators->len;
				for( size_t i=0; i < operators->len; i++ ) {
					/// Match largest operator first.
					if( !strncmp(lexer->iter, ( const char* )(operators->keys[i]), operators->keylens[i]-1) && operator_size < operators->keylens[i]-1 ) {
						operator_size = operators->keylens[i] - 1;
						match = i;
					}
				}
				
				if( match != operators->len ) {
					struct TargumTokenInfo tok = {
						.start    = ( uintptr_t )(lexer->iter - lexer->src.cstr),
						.col      = ( uintptr_t )(lexer->iter - lexer->line_start),
						.line     = lexer->line,
						.filename = &lexer->filename
					};
					const struct HarbolVariant *const v = ( const struct HarbolVariant* )(operators->datum[match]);
					tok.tag = *( const intmax_t* )(v->data);
					harbol_string_add_cstr(&tok.lexeme, ( const char* )(operators->keys[match]));
					lexer->iter += operator_size;
					tok.end = ( uintptr_t )(lexer->iter - lexer->src.cstr);
					harbol_array_insert(&lexer->tokens, &tok, sizeof tok);
					token_count++;
				} else {
					harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "error", &lexer->line, &( const size_t ){ ( uintptr_t )(lexer->iter - lexer->line_start) }, "found no match for symbol(s) '%c' (%u) in operators section.", *lexer->iter, *lexer->iter);
					goto targum_lex_err_exit;
				}
			} else {
				/// Some how we got an operator but operators aren't allowed, shoot error.
				harbol_err_msg(&lexer->err_count, lexer->filename.cstr, "error", &lexer->line, &( const size_t ){ ( uintptr_t )(lexer->iter - lexer->line_start) }, "found symbol(s) '%c' but operator section is missing.", *lexer->iter);
				goto targum_lex_err_exit;
			}
		} /// else operator/comment check.
	} /// while loop.
	
	result = true;
targum_lex_err_exit:;
	struct TargumTokenInfo eof_tok = {
		.start    = ( uintptr_t )(lexer->iter - lexer->src.cstr),
		.col      = ( uintptr_t )(lexer->iter - lexer->line_start),
		.end      = ( uintptr_t )(lexer->iter - lexer->src.cstr),
		.line     = lexer->line,
		.filename = &lexer->filename,
		.lexeme   = harbol_string_make("", &( bool ){false}),
		.tag      = 0
	};
	harbol_array_insert(&lexer->tokens, &eof_tok, sizeof eof_tok);
	return lexer->tokens.len > 1 && result;
}

TARGUM_API bool targum_lexer_remove_token_type(struct TargumLexer *const lexer, const uint32_t tag)
{
	bool deleted_something = false;
	for( size_t i=0; i < lexer->tokens.len; i++ ) {
		struct TargumTokenInfo *const ti = harbol_array_get(&lexer->tokens, i, sizeof *ti);
		if( ti->tag==tag ) {
			harbol_string_clear(&ti->lexeme);
			harbol_array_del_by_index(&lexer->tokens, i, sizeof *ti);
			i = 0;
			deleted_something |= true;
		}
	}
	return deleted_something;
}


TARGUM_API bool targum_lexer_remove_token(struct TargumLexer *const lexer, const uint32_t tag)
{
	for( size_t i=0; i < lexer->tokens.len; i++ ) {
		struct TargumTokenInfo *const ti = harbol_array_get(&lexer->tokens, i, sizeof *ti);
		if( ti->tag==tag ) {
			harbol_string_clear(&ti->lexeme);
			harbol_array_del_by_index(&lexer->tokens, i, sizeof *ti);
			return true;
		}
	}
	return false;
}

TARGUM_API bool targum_lexer_remove_comments(struct TargumLexer *const lexer)
{
	if( lexer->tokens.len==0 || lexer->cfg==NULL ) {
		return false;
	} else {
		const intmax_t *const value_ptr = harbol_cfg_get_int(lexer->cfg, "tokens.comment");
		return( value_ptr==NULL )? false : targum_lexer_remove_token_type(lexer, *value_ptr);
	}
}

TARGUM_API bool targum_lexer_remove_whitespace(struct TargumLexer *const lexer)
{
	bool deleted_something = false;
	if( lexer->tokens.len==0 || lexer->cfg==NULL ) {
		return deleted_something;
	}
	
	struct HarbolMap *const whitespace = harbol_cfg_get_section(lexer->cfg, "tokens.whitespace");
	if( whitespace==NULL ) {
		return deleted_something;
	}
	
	const intmax_t *const whitespaces[] = {
		harbol_cfg_get_int(whitespace, "space"),
		harbol_cfg_get_int(whitespace, "tab"),
		harbol_cfg_get_int(whitespace, "newline")
	};
	
	for( const intmax_t *const *iter = &whitespaces[0]; iter < 1[&whitespaces]; iter++ ) {
		if( *iter==NULL ) {
			continue;
		} else {
			deleted_something |= targum_lexer_remove_token_type(lexer, **iter);
		}
	}
	return deleted_something;
}

TARGUM_API bool targum_lexer_purge_line(struct TargumLexer *const lexer, const size_t line) {
	bool deleted_something = false;
	for( size_t i=0; i < lexer->tokens.len; i++ ) {
		struct TargumTokenInfo *const ti = harbol_array_get(&lexer->tokens, i, sizeof *ti);
		if( ti->line==line ) {
			harbol_string_clear(&ti->lexeme);
			harbol_array_del_by_index(&lexer->tokens, i, sizeof *ti);
			i = 0;
			deleted_something |= true;
		}
	}
	return deleted_something;
}
