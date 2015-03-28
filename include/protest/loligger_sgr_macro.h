/*!
 * \file   loligger_sgr_macro.h
 * \brief  Utility macros to write c-strings with SGR (Select Graphic Rendition).
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2014/12/25  Create.
 * \date   2015/03/26  Add SGR_UNDEF feature.
 * last update: 2015/03/26
 */

/*
 * Usage:
 *   To use macros for SGR, simply #include this header.
 *   No problem occurs if the header is included multiple times.
 *   If you want to undef all macros defined below,
 *   #include the header right after #define SGR_UNDEF.
 *   This means that you can use the header in your library header
 *   without tainting global namespace.
 *
 *   This header contains only C-preprocessor directives,
 *   so you can safely include this file anywhere in your sources.
 */

// NO PRAGMA ONCE!!!
#ifndef	INCLUDED__NU11P0__SPIRAL_OUCH__LOLIGGER_SGR_MACRO_HPP_
#define	INCLUDED__NU11P0__SPIRAL_OUCH__LOLIGGER_SGR_MACRO_HPP_

#define	SGR__CAT(a, b)		a ## b
#define	SGR__CAT2(a, b)		SGR__CAT(a, b)
#define	SGR__SECOND(a, b, ...)	b
#define	SGR__CALL_SECOND(...)	SGR__SECOND(__VA_ARGS__,,)

#define	SGR__SINGLE(x)		SGR_ ## x
#define	SGR__EMPTY()
#define	SGR__EVAL1(...)	__VA_ARGS__
#define	SGR__EVAL2(...)	SGR__EVAL1(SGR__EVAL1(__VA_ARGS__))
#define	SGR__EVAL4(...)	SGR__EVAL2(SGR__EVAL2(__VA_ARGS__))
#define	SGR__EVAL8(...)	SGR__EVAL4(SGR__EVAL4(__VA_ARGS__))
#define	SGR__EVAL16(...)	SGR__EVAL8(SGR__EVAL8(__VA_ARGS__))
#define	SGR__EVAL32(...)	SGR__EVAL16(SGR__EVAL16(__VA_ARGS__))
#define	SGR__EVAL64(...)	SGR__EVAL32(SGR__EVAL32(__VA_ARGS__))
#define	SGR__EVAL128(...)	SGR__EVAL64(SGR__EVAL64(__VA_ARGS__))
#define	SGR__DEFER3(m)	m SGR__EMPTY SGR__EMPTY SGR__EMPTY()()()
// Returns SGR__SEQ_CONCAT_ when arguments 
// 引数が存在すれば SGR__SEQ_CONCAT_ 、空(文字列が与えられていない)であれば SGR__IGNORE を返す
#define	SGR__ARGCHECK(first, ...)	SGR__CALL_SECOND(SGR__CAT(SGR__ARGCHECK_, first) , SGR__DEFER3(SGR__SEQ_CONCAT_)()) (first, __VA_ARGS__)
#define	SGR__ARGCHECK_		~, SGR__IGNORE
#define	SGR__IGNORE(...)
#define	SGR__SEQ(first, ...)	SGR__SINGLE(first) SGR__ARGCHECK(__VA_ARGS__,)
#define	SGR__SEQ_CONCAT(first, ...)	";" SGR__SINGLE(first) SGR__ARGCHECK(__VA_ARGS__,)
#define	SGR__SEQ_CONCAT_()	SGR__SEQ_CONCAT

/*
 * Macros for user
 */
// SGR (Select Graphic Rendition) begin with CSI (Control Sequence Introducer).
#define		SGR(...)	SGR_BEGIN__ SGR__EVAL128(SGR__SEQ(__VA_ARGS__,)) SGR_END__

/*
 * User can define other SGR macro.
 * For example, if you would like to use italic font, define as below:
 *     #define SGR_ITALIC	"3"
 * now you can use ITALIC in SGR() macro, like SGR(RESET, ITALIC, FG_RED) .
 * Note that SGR_UNDEF is reserved for the other purpose.
 */
/*
 * For detail, see http://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes ,
 * "SGR (Select Graphic Rendition) parameters" table (2014-12-26).
 */

// SGR_BEGIN__ is CSI.
#define		SGR_BEGIN__			"\033["
#define		SGR_END__			"m"
#define		SGR_FG_BLACK		"30"
#define		SGR_FG_RED			"31"
#define		SGR_FG_GREEN		"32"
#define		SGR_FG_YELLOW		"33"
#define		SGR_FG_BLUE			"34"
#define		SGR_FG_MAGENTA		"35"
#define		SGR_FG_CYAN			"36"
#define		SGR_FG_WHITE		"37"
#define		SGR_FG_DEFAULT		"39"
#define		SGR_BG_BLACK		"40"
#define		SGR_BG_RED			"41"
#define		SGR_BG_GREEN		"42"
#define		SGR_BG_YELLOW		"43"
#define		SGR_BG_BLUE			"44"
#define		SGR_BG_MAGENTA		"45"
#define		SGR_BG_CYAN			"46"
#define		SGR_BG_WHITE		"47"
#define		SGR_BG_DEFAULT		"49"
#define		SGR_RESET			"0"
#define		SGR_BOLD			"1"
#define		SGR_UNDERLINE		"4"
#define		SGR_BLINK			"5"
#define		SGR_NEGATIVE		"7"
#define		SGR_NO_BOLD			"22"
#define		SGR_NO_UNDERLINE	"24"
#define		SGR_NO_NEGATIVE		"27"

#elif	defined(SGR_UNDEF)	// ifndef INCLUDED__NU11P0__SPIRAL_OUCH__LOLIGGER_SGR_MACRO_HPP_
// If SGR_UNDEF is defined, undef all macro defined at the first include.

#undef	SGR__CAT
#undef	SGR__CAT2
#undef	SGR__SECOND
#undef	SGR__CALL_SECOND

#undef	SGR__SINGLE
#undef	SGR__EMPTY
#undef	SGR__EVAL1
#undef	SGR__EVAL2
#undef	SGR__EVAL4
#undef	SGR__EVAL8
#undef	SGR__EVAL16
#undef	SGR__EVAL32
#undef	SGR__EVAL64
#undef	SGR__EVAL128
#undef	SGR__DEFER3
#undef	SGR__ARGCHECK
#undef	SGR__ARGCHECK_
#undef	SGR__IGNORE
#undef	SGR__SEQ
#undef	SGR__SEQ_CONCAT
#undef	SGR__SEQ_CONCAT_

#undef		SGR

#undef		SGR_BEGIN__
#undef		SGR_END__
#undef		SGR_FG_BLACK
#undef		SGR_FG_RED
#undef		SGR_FG_GREEN
#undef		SGR_FG_YELLOW
#undef		SGR_FG_BLUE
#undef		SGR_FG_MAGENTA
#undef		SGR_FG_CYAN
#undef		SGR_FG_WHITE
#undef		SGR_FG_DEFAULT
#undef		SGR_BG_BLACK
#undef		SGR_BG_RED
#undef		SGR_BG_GREEN
#undef		SGR_BG_YELLOW
#undef		SGR_BG_BLUE
#undef		SGR_BG_MAGENTA
#undef		SGR_BG_CYAN
#undef		SGR_BG_WHITE
#undef		SGR_BG_DEFAULT
#undef		SGR_RESET
#undef		SGR_BOLD
#undef		SGR_UNDERLINE
#undef		SGR_BLINK
#undef		SGR_NEGATIVE
#undef		SGR_NO_BOLD
#undef		SGR_NO_UNDERLINE
#undef		SGR_NO_NEGATIVE

#undef		SGR_UNDEF

#endif	// ifndef INCLUDED__NU11P0__SPIRAL_OUCH__LOLIGGER_SGR_MACRO_HPP_
