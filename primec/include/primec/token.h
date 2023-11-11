
/**
 * @file token.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-12
 */

#ifndef __primec__include__primec__token_h__
#define __primec__include__primec__token_h__

#include <stdint.h>

typedef struct
{
	const char* file;
	uint64_t line;
	uint64_t column;
} primec_location_s;

typedef enum
{
	T_ATTR_FINI,
	T_ATTR_INIT,
	T_ATTR_OFFSET,
	T_ATTR_PACKED,
	T_ATTR_SYMBOL,
	T_ATTR_TEST,
	T_ATTR_THREADLOCAL,
	T_UNDERSCORE,
	T_ABORT,
	T_ALIGN,
	T_ALLOC,
	T_APPEND,
	T_AS,
	T_ASSERT,
	T_BOOL,
	T_BREAK,
	T_CASE,
	T_CONST,
	T_CONTINUE,
	T_DEF,
	T_DEFER,
	T_DELETE,
	T_ELSE,
	T_ENUM,
	T_EXPORT,
	T_F32,
	T_F64,
	T_FALSE,
	T_FN,
	T_FOR,
	T_FREE,
	T_I16,
	T_I32,
	T_I64,
	T_I8,
	T_IF,
	T_INSERT,
	T_INT,
	T_IS,
	T_LEN,
	T_LET,
	T_MATCH,
	T_NEVER,
	T_NULL,
	T_NULLABLE,
	T_OFFSET,
	T_OPAQUE,
	T_RETURN,
	T_RUNE,
	T_SIZE,
	T_STATIC,
	T_STR,
	T_STRUCT,
	T_SWITCH,
	T_TRUE,
	T_TYPE,
	T_U16,
	T_U32,
	T_U64,
	T_U8,
	T_UINT,
	T_UINTPTR,
	T_UNION,
	T_USE,
	T_VAARG,
	T_VAEND,
	T_VALIST,
	T_VASTART,
	T_VOID,
	T_YIELD,
	T_LAST_KEYWORD = T_YIELD,

	// Operators
	T_ARROW,
	T_BANDEQ,
	T_BAND,
	T_BNOT,
	T_BOR,
	T_COLON,
	T_COMMA,
	T_DIV,
	T_DIVEQ,
	T_DOT,
	T_DOUBLE_COLON,
	T_ELLIPSIS,
	T_EQUAL,
	T_GREATER,
	T_GREATEREQ,
	T_LAND,
	T_LANDEQ,
	T_LBRACE,
	T_LBRACKET,
	T_LEQUAL,
	T_LESS,
	T_LESSEQ,
	T_LNOT,
	T_LOR,
	T_LOREQ,
	T_LPAREN,
	T_LSHIFT,
	T_LSHIFTEQ,
	T_LXOR,
	T_LXOREQ,
	T_MINUS,
	T_MINUSEQ,
	T_MODEQ,
	T_MODULO,
	T_NEQUAL,
	T_BOREQ,
	T_PLUS,
	T_PLUSEQ,
	T_QUESTION,
	T_RBRACE,
	T_RBRACKET,
	T_RPAREN,
	T_RSHIFT,
	T_RSHIFTEQ,
	T_SEMICOLON,
	T_SLICE,
	T_TIMES,
	T_TIMESEQ,
	T_BXOR,
	T_BXOREQ,
	T_LAST_OPERATOR = T_BXOREQ,

	// Tokens with additional information
	T_LITERAL,
	T_NAME,

	// Magic tokens
	primec_token_eof,
	primec_token_none
} primec_token_e;

typedef enum
{
	// Built-in types
	// The order of these is important
	STORAGE_BOOL,
	STORAGE_F32,
	STORAGE_F64,
	STORAGE_I16,
	STORAGE_I32,
	STORAGE_I64,
	STORAGE_I8,
	STORAGE_INT,
	STORAGE_NEVER,
	STORAGE_NULL,
	STORAGE_OPAQUE,
	STORAGE_RUNE,
	STORAGE_SIZE,
	STORAGE_STRING,
	STORAGE_U16,
	STORAGE_U32,
	STORAGE_U64,
	STORAGE_U8,
	STORAGE_UINT,
	STORAGE_UINTPTR,
	STORAGE_VOID,
	// Other types
	STORAGE_ALIAS,
	STORAGE_ARRAY,
	STORAGE_ENUM,
	STORAGE_FUNCTION,
	STORAGE_POINTER,
	STORAGE_SLICE,
	STORAGE_STRUCT,
	STORAGE_TAGGED,
	STORAGE_TUPLE,
	STORAGE_UNION,
	STORAGE_VALIST,
	STORAGE_FCONST,
	STORAGE_ICONST,
	STORAGE_RCONST,
	// For internal use only
	STORAGE_ERROR,
} primec_storage_e;

typedef struct
{
	primec_location_s location;
	primec_token_e type;
	primec_storage_e storage;
	union
	{
		char* name;
		uint32_t rune;
		int64_t ival;
		uint64_t uval;
		double fval;
		struct
		{
			uint64_t len;
			char* value;
		} string;
	};
} primec_token_s;

#endif
