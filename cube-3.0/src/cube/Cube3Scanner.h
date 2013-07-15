#ifndef CUBE3SCANNER_H
#define CUBE3SCANNER_H

// Flex expects the signature of yylex to be defined in the macro YY_DECL, and
// the C++ parser expects it to be declared. We can factor both as follows.

#ifndef YY_DECL

#define	YY_DECL						\
    cubeparser::Cube3Parser::token_type				\
    cubeparser::Cube3Scanner::lex(				\
	cubeparser::Cube3Parser::semantic_type* yylval,		\
	cubeparser::Cube3Parser::location_type* yylloc		\
    )
#endif

#ifndef __FLEX_LEXER_H
//#define yyFlexLexer Cube3FlexLexer
#include "FlexLexer.h"
#undef yyFlexLexer
#endif

#include "Cube3Parser.h"

namespace cubeparser {


/** Scanner is a derived class to add some extra function to the scanner
 * class. Flex itself creates a class named yyFlexLexer, which is renamed using
 * macros to CubeparserFlexLexer. However we change the context of the generated
 * yylex() function to be contained within the Scanner class. This is required
 * because the yylex() defined in yyFlexLexer has no parameters. */
class Cube3Scanner : public yyFlexLexer
{
public:
    /** Create a new scanner object. The streams arg_yyin and arg_yyout default
     * to cin and cout, but that assignment is only made when initializing in
     * yylex(). */
    Cube3Scanner(std::istream* arg_yyin = 0,
	    std::ostream* arg_yyout = 0,
	    ParseContext* parseContext = NULL);

    /** Required for virtual functions */
    virtual ~Cube3Scanner();

    /** This is the main lexing function. It is generated by flex according to
     * the macro declaration YY_DECL above. The generated bison parser then
     * calls this virtual function to fetch new tokens. */
    virtual Cube3Parser::token_type lex(
	Cube3Parser::semantic_type* yylval,
	Cube3Parser::location_type* yylloc
	);

 private:

    ParseContext* parseContext;

};

} // namespace cubeparser

#endif // CUBE3SCANNER_H
