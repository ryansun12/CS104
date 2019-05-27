#ifndef __SYM_H__
#define __SYM_H__

#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <iostream>
#include "astree.h"

using namespace std;

struct symbol;

using symbol_table = unordered_map<string*, symbol*>;
using symbol_entry = symbol_table::value_type;

struct symbol {
    size_t sequence;
    location lloc;
    attr_bitset attributes;
    vector<symbol*>* parameters; // = nullptr;
    symbol_table* fields;
    size_t block_nr; // = 0;
    string* struct_name; // = nullptr;
};
// function declarations
void traversal(FILE* out, astree* node);
void handle_func(FILE*out, astree* node);
void handle_struct(FILE*out, astree* node);
void handle_vardecl(FILE*out, astree* node);
void set_attr(astree* node, symbol* sym);
#endif

