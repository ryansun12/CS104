// $Id: parser.y,v 1.14 2016-10-06 16:26:41-07 - - $
// Code skeleton copied from parser.y
// Ryan Sun 1609724 Asg3 ryjsun@ucsc.edu

%{

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "lyutils.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose
%initial-action {
   parser::root = new astree (TOK_ROOT, {0, 0, 0}, "ROOT");
}

%token TOK_VOID TOK_INT TOK_STRING 
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULLPTR TOK_ARRAY TOK_ARROW TOK_ALLOC TOK_PTR
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE TOK_NOT
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON
%token TOK_ROOT TOK_BLOCK TOK_CALL TOK_INITDECL
%token TOK_POS TOK_NEG TOK_TYPE_ID
%token TOK_PARAM TOK_VARDECL
%token TOK_INDEX TOK_FUNCTION

%right  TOK_IF TOK_ELSE 
%right  '=' 
%left   TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left   '+' '-'
%left   '*' '/' '%'
%right  U_PLUS U_MINUS TOK_NOT 
%left   TOK_ARROW TOK_ALLOC '[' '('

%start  start

%%

start   : program               { yyparse_astree = $1; }
        ;

program : program structdef     { $$ = $1->adopt ($2); }
        | program function      { $$ = $1->adopt ($2); }
        | program statement     { $$ = $1->adopt ($2); } 
        | program error '}'     { $$ = $1; }
        | program error ';'     { $$ = $1; } 
        |                       { $$ = parser::root; } 
        ;

structdef : TOK_STRUCT TOK_IDENT '{' '}' ';' {
            destroy($3,$4); destroy($5);
            $$ = $1->adopt($2); }
          | TOK_STRUCT TOK_IDENT '{' structs '}'';' {
            destroy($3,$5); destroy($6);
            $$ = $1->adopt($2,$4); }
          ;

structs   : structs tid ';' {
            destroy($3);
            $$ = $1->adopt($2); }
          | tid ';'          {destroy($2); $$ = $1; }
 
          ;

tid       : type TOK_IDENT {
            $$ = new astree(TOK_TYPE_ID, $1->lloc, "");
            $$ = $$->adopt($1,$2); }
                   ;

type      : plaintype    { $$ = $1;}
          | TOK_ARRAY TOK_LT plaintype TOK_GT {
            destroy($2, $4);
            $$ = $1->adopt($3); }
          ;

plaintype : TOK_VOID     { $$ = $1;}
          | TOK_INT      { $$ = $1;}
          | TOK_STRING   { $$ = $1;}
          | TOK_PTR TOK_LT TOK_STRUCT TOK_IDENT TOK_GT {
            destroy($2,$3);
            destroy($5);
            $$ = $1->adopt($4); }
         ;          

function  : tid '(' ')' block {
            destroy($3);
            $2->adopt_sym(nullptr, TOK_PARAM);
            $$ = new astree(TOK_FUNCTION, $1->lloc, "");
            $$ = $$->adopt($1,$2); 
            $$ = $$->adopt($4); }
          | tid  params ')' block {
            destroy($3);
            $$ = new astree(TOK_FUNCTION, $1->lloc, "");
            $$ = $$->adopt($1,$2);
            $$ = $$->adopt($4); }          
          ;

params    : params ',' tid  {
            destroy($2);
            $$ = $1 ->adopt($3); }
          | '(' tid    { 
            $$ = $1->adopt_sym($2,TOK_PARAM); } 
          ;

block     : statements '}' { destroy($2);
            $$ = $1; }
          | '{' '}' {
            destroy($2);
            $$ = $1->adopt_sym(nullptr, TOK_BLOCK); }
          | ';'     {$$ = nullptr; } 
          ;

statements: statements statement{ $$ = $1->adopt($2);}
          | '{' statement { $$ = $1->adopt_sym($2, TOK_BLOCK);}
          ;

statement : vardecl { $$ = $1;}
          | block   { $$ = $1;}
          | while   { $$ = $1;}
          | ifelse  { $$ = $1;}
          | return  { $$ = $1;}
          | expr ';'{ destroy($2); $$ = $1; }
          ;

vardecl   : type TOK_IDENT '=' expr ';' {
            destroy($5); 
            $3->adopt_sym($1,TOK_TYPE_ID);
            $$ = $3->adopt($2,$4);}
          ;

while     : TOK_WHILE '(' expr ')' statement {
            destroy($2,$4);
            $$ = $1->adopt($3,$5);}
          ;

ifelse    : TOK_IF '(' expr ')' statement TOK_ELSE statement {
            destroy($2,$4); destroy($6);
            $1->adopt($3,$5);
            $$ = $1->adopt($7);}
          | TOK_IF '(' expr ')' statement {
            destroy($2,$4);
            $$ = $1->adopt($3, $5); }
          ;

return    : TOK_RETURN ';' {
            destroy($2);
            $$ = $1; }
          | TOK_RETURN expr ';' {
            destroy($3);
            $$ = $1->adopt($2);}
          ;

expr      : expr BINOP expr    {$$ = $2->adopt($1,$3);}
          | UNOP expr          {$$ = $1->adopt($2);}
          | allocator          {$$ = $1;}
          | call               {$$ = $1;}
          | '(' expr ')'       {destroy($1,$3); $$ = $2;}
          | variable           {$$ = $1;}
          | constant           {$$ = $1;}
          ;

BINOP     : '='    { $$ = $1;}
          | TOK_EQ { $$ = $1;}
          | TOK_NE { $$ = $1;} 
          | TOK_LT { $$ = $1;}
          | TOK_LE { $$ = $1;}
          | TOK_GT { $$ = $1;}
          | TOK_GE { $$ = $1;}
          |  '+'   { $$ = $1;}
          |  '-'   { $$ = $1;}
          |  '*'   { $$ = $1;}
          |  '/'   { $$ = $1;}
          |  '%'   { $$ = $1;}
          | TOK_ARROW { $$ = $1; }
          ;

UNOP      : '+' expr     { $$ = $1->adopt_sym($2,U_PLUS);}
          | '-' expr     { $$ = $1->adopt_sym($2,U_MINUS);}
          | TOK_NOT expr { $$ = $1->adopt($2);}
          ;

allocator : TOK_ALLOC TOK_LT TOK_STRING TOK_GT '(' expr ')' {
            destroy($2,$4);
            destroy($5, $7);
            $$ = $1->adopt($3,$6); }
          | TOK_ALLOC TOK_LT TOK_STRUCT TOK_IDENT TOK_GT '(' ')' {
            destroy($2,$3);
            destroy($5,$6);
            destroy($7);
            $$ = $1->adopt($4); }
          | TOK_ALLOC TOK_LT TOK_ARRAY TOK_LT plaintype TOK_GT
            TOK_GT '(' expr ')'{
            destroy($2, $4);
            destroy($6, $7);
            destroy($8, $10);
            $3->adopt($5);
            $$ = $1->adopt($3,$9); }
          ;

call      : args ')' {
            destroy($2);
            $$ = $1; }
          | TOK_IDENT '(' ')' {
            destroy($3);
            $$ = $2->adopt_sym($1, TOK_CALL); }
          ;

args      : args ',' expr { 
            destroy($2);
            $$ = $1->adopt($3); }
          | TOK_IDENT '(' expr { 
            $2->adopt_sym($1, TOK_CALL);
            $$ = $2->adopt($3); } 
          ;

variable  : TOK_IDENT  { $$ = $1;}
          | expr '[' expr ']' {
            destroy($4);
            $2->adopt_sym($1,TOK_INDEX);
            $$ = $2->adopt($3); }
          | expr TOK_ARROW TOK_IDENT {
            $$ = $3->adopt($1,$2); }
          ;

constant  : TOK_INTCON    {$$ = $1;}
          | TOK_CHARCON   {$$ = $1;}
          | TOK_STRINGCON {$$ = $1;}
          | TOK_NULLPTR   {$$ = $1;}
          ;
%%

const char* parser::get_tname (int symbol) {
  return yytname [YYTRANSLATE (symbol)];
}

