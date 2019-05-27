/* Ryan Sun
* 1609724
* asg2 scanner
some code copied from lexer.l */

%{

#include "lyutils.h"
#include "astree.h"
#define YY_USER_ACTION  { lexer::advance(); }
int yylval_token (int tok) {
  yylval = new astree(tok, lexer::lloc, yytext);
  fprintf(tokfile, "%3lu %2lu.%03lu %4u  %-15s (%s)\n ", 
           lexer::lloc.filenr, lexer::lloc.linenr, 
           lexer::lloc.offset, tok,  
           parser::get_tname(tok), yytext);
   return tok;}
%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
NUMBER          {DIGIT}+
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
NOTIDENT        ({NUMBER}+{IDENT})
CHAR            ('([^\\'\n]|\\[\\'"0nt])')
STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")
NOTSTRING       (\"[^\n\"\\]*(.|\n))
NOTCHAR         ('(.*)|\n')
BADCHAR         ('[\n\'\\]'|'.[^\'])

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"if"           { return yylval_token (TOK_IF);     }
"else"         { return yylval_token (TOK_ELSE);   }
"while"        { return yylval_token (TOK_WHILE);  }
"return"       { return yylval_token (TOK_RETURN); }
"alloc"        { return yylval_token (TOK_ALLOC);    }
"nullptr"      { return yylval_token (TOK_NULLPTR); }
"not"          { return yylval_token (TOK_NOT); }
"int"          { return yylval_token (TOK_INT);   }
"string"       { return yylval_token (TOK_STRING);    }
"struct"       { return yylval_token (TOK_STRUCT);    }
"array"        { return yylval_token (TOK_ARRAY);   }
"ptr"          { return yylval_token (TOK_PTR);   }
"void"         { return yylval_token (TOK_VOID);    }

"+"            { return yylval_token ('+');  }
"-"            { return yylval_token ('-');  }
"*"            { return yylval_token ('*');  }
"/"            { return yylval_token ('/');  }
"%"            { return yylval_token ('%');  }
"="            { return yylval_token ('=');  }
"=="           { return yylval_token (TOK_EQ); }
"!="           { return yylval_token (TOK_NE); }
"<"            { return yylval_token (TOK_LT); }
"<="           { return yylval_token (TOK_LE); }
">"            { return yylval_token (TOK_GT); }
">="           { return yylval_token (TOK_GE); }
"->"           { return yylval_token (TOK_PTR); }
","            { return yylval_token (',');  }
";"            { return yylval_token (';');  }
"("            { return yylval_token ('(');  }
")"            { return yylval_token (')');  }
"["            { return yylval_token ('[');  }
"]"            { return yylval_token (']');  }
"{"            { return yylval_token ('{');  }
"}"            { return yylval_token ('}');  }
"[]"           { return yylval_token (TOK_ARRAY);  }

{NUMBER}       { return yylval_token (TOK_INTCON); }
{IDENT}        { return yylval_token (TOK_IDENT); }
{NOTIDENT}     { lexer::badtoken(yytext); }
{CHAR}         { return yylval_token (TOK_CHARCON); }
{STRING}       { return yylval_token (TOK_STRINGCON);}
{BADCHAR}      { lexer::badtoken(yytext); } 
{NOTSTRING}    { lexer::badtoken(yytext); }
.              { lexer::badchar (*yytext); }

%%
