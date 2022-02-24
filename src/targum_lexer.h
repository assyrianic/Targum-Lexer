#ifndef TARGUM_LEXER_INCLUDED
#	define TARGUM_LEXER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "map/map.h"
#include "cfg/cfg.h"

#define TARGUM_LEXER_VERSION_MAJOR    1
#define TARGUM_LEXER_VERSION_MINOR    3
#define TARGUM_LEXER_VERSION_PATCH    0
#define TARGUM_LEXER_VERSION_RELEASE  "beta"
#define STR_HELPER(x)                 #x
#define STR(x)                        STR_HELPER(x)
#define TARGUM_LEXER_VERSION_STRING \
			STR(TARGUM_LEXER_VERSION_MAJOR) "." STR(TARGUM_LEXER_VERSION_MINOR) "." STR(TARGUM_LEXER_VERSION_PATCH) " " TARGUM_LEXER_VERSION_RELEASE

#ifdef TARGUM_DLL
#	ifndef TARGUM_LIB 
#		define TARGUM_API __declspec(dllimport)
#	else
#		define TARGUM_API __declspec(dllexport)
#	endif
#else
#	define TARGUM_API
#endif


struct TargumTokenInfo {
	uint32_t
		start,  /// start offset of lexeme.
		end,    /// end offset of lexeme.
		line,   /// line in source code.
		col,    /// column in source code.
		tag
	;
};

TARGUM_API NO_NULL uint32_t targum_token_info_get_token(const struct TargumTokenInfo *tokinfo);
TARGUM_API NO_NULL size_t   targum_token_info_get_len(const struct TargumTokenInfo *tokinfo);


struct TargumLexer {
	struct HarbolArray      tokens;
	struct HarbolString     filename, src;
	struct HarbolMap       *cfg;
	char                   *iter, *line_start;
	size_t                  line, index, err_count;
	struct TargumTokenInfo *curr_tok;
};

TARGUM_API NEVER_NULL(1) struct TargumLexer *targum_lexer_new_from_buffer(const char src[], struct HarbolMap *cfg);
TARGUM_API NEVER_NULL(1) struct TargumLexer *targum_lexer_new_from_file(const char filename[], struct HarbolMap *cfg);

TARGUM_API NEVER_NULL(1) struct TargumLexer targum_lexer_make_from_buffer(const char src[], struct HarbolMap *cfg);
TARGUM_API NEVER_NULL(1,3) struct TargumLexer targum_lexer_make_from_file(const char filename[], struct HarbolMap *cfg, bool *result);

TARGUM_API NO_NULL void targum_lexer_clear(struct TargumLexer *lexer, bool free_cfg);
TARGUM_API NO_NULL void targum_lexer_free(struct TargumLexer **lexer_ref, bool free_cfg);
TARGUM_API NO_NULL void targum_lexer_flush_tokens(struct TargumLexer *lexer);

TARGUM_API NO_NULL bool targum_lexer_load_cfg_file(struct TargumLexer *lexer, const char cfg_file[]);
TARGUM_API NO_NULL bool targum_lexer_load_cfg_cstr(struct TargumLexer *lexer, const char cfg_cstr[]);
TARGUM_API NO_NULL struct HarbolMap *targum_lexer_get_cfg(const struct TargumLexer *lexer);

TARGUM_API NO_NULL const char *targum_lexer_get_filename(const struct TargumLexer *lexer);
TARGUM_API NO_NULL size_t targum_lexer_get_token_index(const struct TargumLexer *lexer);
TARGUM_API NO_NULL size_t targum_lexer_get_token_count(const struct TargumLexer *lexer);
TARGUM_API NO_NULL size_t targum_lexer_get_err_count(const struct TargumLexer *lexer);

TARGUM_API NO_NULL struct TargumTokenInfo *targum_lexer_get_token(const struct TargumLexer *lexer);
TARGUM_API NO_NULL struct TargumTokenInfo *targum_lexer_peek_token(const struct TargumLexer *lexer, size_t lookahead);
TARGUM_API NO_NULL struct TargumTokenInfo *targum_lexer_advance(struct TargumLexer *lexer, bool flush_tokens);

TARGUM_API NO_NULL void targum_lexer_reset(struct TargumLexer *lexer);
TARGUM_API NO_NULL void targum_lexer_reset_token_index(struct TargumLexer *lexer);

TARGUM_API NO_NULL bool targum_lexer_generate_tokens(struct TargumLexer *lexer);

TARGUM_API NO_NULL bool targum_lexer_remove_token_type(struct TargumLexer *lexer, uint32_t tag);
TARGUM_API NO_NULL bool targum_lexer_remove_token(struct TargumLexer *lexer, uint32_t tag);
TARGUM_API NO_NULL bool targum_lexer_remove_comments(struct TargumLexer *lexer);
TARGUM_API NO_NULL bool targum_lexer_remove_whitespace(struct TargumLexer *lexer);

TARGUM_API NO_NULL bool targum_lexer_purge_line(struct TargumLexer *lexer, size_t line);
TARGUM_API NO_NULL char *targum_lexer_get_lexeme(const struct TargumLexer *lexer, const struct TargumTokenInfo *tokinfo);


#ifdef __cplusplus
}
#endif

#endif /** TARGUM_LEXER_INCLUDED */
