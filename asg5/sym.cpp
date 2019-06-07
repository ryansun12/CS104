// Ryan Sun
// ryjsun@ucsc.edu
// Assignment 4 Symbol Table & Type Checking

#include "astree.h"
#include "sym.h"
#include "lyutils.h"

symbol_table global;

symbol_table structs;

size_t next_block = 0;
vector<int> block_stack;
extern FILE *symfile;

void print_symbol(FILE*, string*, symbol*, int);

void traversal(FILE* out, astree* node) {
// printf("DEBUG>>> start AST table traversal...\n");
   if (node == nullptr || node->symbol != TOK_ROOT) 
     { 
       errprintf("file error"); return;
     } 

// printf("DEBUG>>> Root has %0d children\n", node->children.size());
   for(uint i = 0; i < node->children.size(); i++) { 
    switch(node->children[i]->symbol) {
     case TOK_FUNCTION:
          handle_func(out, node->children[i]); 
          break;
     case TOK_STRUCT:
          handle_struct(out, node->children[i]); 
          break;
     case TOK_TYPE_ID:
     case TOK_VARDECL:
          handle_vardecl(out, node->children[i]); 
          break;
     default: break;
    }
   }
// printf("DEBUG>>> AST table traversal completed\n");
} 

void handle_func(FILE* out, astree* node) {
//   printf("DEBUG>>> start function handling...\n");
   node->attributes[static_cast<unsigned>(attr::FUNCTION)] = 1;
   switch(node->children[0]->children[0]->symbol){
     case TOK_INT: {
          node->attributes[static_cast<unsigned>(attr::INT)] = 1;
          break; 
          }
     case TOK_STRING: {
          node->attributes[static_cast<unsigned>(attr::STRING)] = 1;
          break; 
          }
     case TOK_NULLPTR: {
          node->attributes[static_cast<unsigned>(attr::NULLPTR_T)] = 1;
          break; 
          }
     case TOK_VOID: {
          node->attributes[static_cast<unsigned>(attr::VOID)] = 1;
          break; 
          }
    // case TOK_PTR: {
     //     node->attributes[static_cast<unsigned>(attr::PTR)] = 1;
     //     break; 
     //     }
    }
   string* name = 
   const_cast<string*>(node->children[0]->children[1]->lexinfo);
   if (global.find(name) != global.end()) {
      errprintf("duplicate function");
      return;
   }
   next_block++;
// printf("DEBUG>>> block index = %0d\n", static_cast<int>(next_block));

   symbol* sym = new symbol;
   sym->attributes = node->attributes;
   sym->parameters = new vector<symbol*>;
   sym->lloc = node->lloc;
   sym->block_nr = 0;
   symbol_entry ent(name, sym);
   global.insert(ent);

   symbol_table local;

// printf("DEBUG>>> Function has %0d children\n",node->children.size());
   for(uint i=0; i<node->children.size(); i++) {
// printf("DEBUG>>> function child[%0d]...\n", static_cast<int>(i));
    astree* child = node->children[i];
    switch(child->symbol) {
     case TOK_TYPE_ID:
         // printf("DEBUG>>> function type_id...\n");
          // print function declaration
          print_symbol(out, name, sym, 0);
         // printf("DEBUG>>> function type_id completed\n");
          break;
     case TOK_PARAM: 
          if (child->children.size() > 0) {
//  printf("DEBUG>>> Param has %0ud children\n",child->children.size());
           for (uint j=0; j<child->children.size(); j++) {
            astree* childchild = child->children[j];
         // printf("DEBUG>>> Param child[%0d] has %0ud children...\n",
          //         static_cast<int>(j),
           //        childchild->children.size());
            // create new symbol for each parameter
            symbol* par_sym = new symbol;
            set_attr(childchild->children[0], par_sym);
           par_sym->attributes[static_cast<unsigned>(attr::VARIABLE)]=1;
           par_sym->attributes[static_cast<unsigned>(attr::PARAM)]=1;
           par_sym->attributes[static_cast<unsigned>(attr::LVAL)]=1;
           if (par_sym->attributes[static_cast<unsigned>(attr::PTR)] ||
               par_sym->attributes[static_cast<unsigned>(attr::STRUCT)])
              par_sym->struct_name =
               const_cast<string*>(childchild->children[0]->
                                   children[0]->lexinfo);
            par_sym->sequence = j;
            par_sym->lloc = childchild->lloc;
            par_sym->block_nr = next_block;
            // push symbol to vector
            sym->parameters->push_back(par_sym);
            // print parameters
            print_symbol(out,
                 const_cast<string*>(childchild->children[1]->lexinfo),
                 par_sym, 1);
          //  printf("DEBUG>>> Param child[%0d] completed\n",
         //          static_cast<int>(j));
           }
          }
          break;
     case TOK_BLOCK: 
          if (child->children.size() > 0){
           for (uint j=0; j<child->children.size(); j++) {
          //  printf("DEBUG>>> Block child[%0d]...\n",
           //        static_cast<int>(j));
            if (child->children[j]->symbol == TOK_TYPE_ID) {
             astree* childchild = child->children[j];
             // create new symbol for each variable
             symbol* loc_sym = new symbol;
             set_attr(childchild->children[0], loc_sym);
         loc_sym->attributes[static_cast<unsigned>(attr::VARIABLE)] = 1;
            loc_sym->attributes[static_cast<unsigned>(attr::LOCAL)] = 1;
             loc_sym->attributes[static_cast<unsigned>(attr::LVAL)] = 1;
           if (loc_sym->attributes[static_cast<unsigned>(attr::PTR)] ||
               loc_sym->attributes[static_cast<unsigned>(attr::STRUCT)])
              loc_sym->struct_name =
               const_cast<string*>(childchild->children[0]->
                                   children[0]->lexinfo);
             loc_sym->sequence = j;
             loc_sym->lloc = childchild->lloc;
             loc_sym->block_nr = next_block;
             // insert to local symbol table
             string* loc_name =
             const_cast<string*>(childchild->children[1]->lexinfo);
             symbol_entry loc_ent(loc_name, loc_sym);
             local.insert(loc_ent);
             // print variables
             print_symbol(out, loc_name, loc_sym, 1);
            }
         //   printf("DEBUG>>> Block child[%0d] completed\n",
          //         static_cast<int>(j));
           else if (child->children[j]->symbol == '='){
              //todo ???
             }
           }
          }
          if(type_check(child) == false) { 
           errprintf("type check error"); return; 
          }
          break;
     default: break;
    } 
   }
   fprintf(out, "\n");
//   printf("DEBUG>>> function handling completed\n");
}

void handle_struct(FILE* out, astree* node) {
   node->attributes[static_cast<unsigned>(attr::STRUCT)] = 1;
   string* name = 
   const_cast<string*>(node->children[0]->lexinfo);
   if (global.find(name) != global.end()) {
      errprintf("duplicate struct");
      return;
   }
   next_block++;

   symbol* sym = new symbol;
   symbol_table fd_symtbl;
   // work on global symbol table
   sym->attributes = node->attributes;
   sym->struct_name = name;
   if (node->children.size() > 1)
     sym->fields = &fd_symtbl;
   else
     sym->fields = nullptr;
   sym->lloc = node->lloc;
   sym->block_nr = 0;
   symbol_entry ent(name, sym);
   global.insert(ent);
   print_symbol(out, name, sym, 0);
   // work on fields symbol table
   if (node->children.size() > 1) {
    for(uint i=1; i<node->children.size(); i++) {
     astree* child = node->children[i];
     symbol* fsym = new symbol;
     set_attr(child->children[0], fsym);
     string* fname = const_cast<string*>(child->children[1]->lexinfo);
     fsym->attributes[static_cast<unsigned>(attr::FIELD)] = 1;
     if (fsym->attributes[static_cast<unsigned>(attr::PTR)] ||
         fsym->attributes[static_cast<unsigned>(attr::STRUCT)])
      fsym->struct_name = fname;
     fsym->sequence = i-1;
     fsym->lloc = child->lloc;
     fsym->block_nr = next_block;
     symbol_entry fent(fname, fsym);
     fd_symtbl.insert(fent);
     print_symbol(out, fname, fsym, 1);
    }
   }
   fprintf(out, "\n");
}

void handle_vardecl(FILE* out, astree* node) {
   node->attributes[static_cast<unsigned>(attr::VARIABLE)] = 1;
   string* name = const_cast<string*>(node->children[1]->lexinfo);
   if (global.find(name) != global.end()) {
      errprintf("duplicate variable");
      return;
   }

   symbol* sym = new symbol;
   sym->attributes = node->attributes;
   set_attr(node->children[0], sym);
   if (sym->attributes[static_cast<unsigned>(attr::PTR)] ||
       sym->attributes[static_cast<unsigned>(attr::STRUCT)])
    sym->struct_name =
      const_cast<string*>(node->children[0]->children[0]->lexinfo);
   sym->lloc = node->lloc;
   sym->block_nr = 0;
   symbol_entry ent(name, sym);
   global.insert(ent);

   print_symbol(out, name, sym, 0);
}

void print_symbol(FILE* out, string* name, symbol* symb, int indent) {
  if (indent) {
   fprintf(out, "   ");
  }

  attr_bitset a = symb->attributes;
  string string_list = astree::attr_string(a, 
                       symb->struct_name);
  if (symb->attributes[static_cast<unsigned>(attr::FUNCTION)]) {
    fprintf(out, "%s (%zu.%zu.%zu) {0} %s\n",
            (*name).c_str(),
            symb->lloc.filenr,
            symb->lloc.linenr,
            symb->lloc.offset,
            string_list.c_str());
   }
  else if (symb->attributes[static_cast<unsigned>(attr::STRUCT)]) {
    //struct
    fprintf(out, "%s (%zu.%zu.%zu) {0} %s\n",
            (*name).c_str(),
            symb->lloc.filenr,
            symb->lloc.linenr,
            symb->lloc.offset,
            string_list.c_str());
   }
  else {
    //variables
    if (symb->block_nr == 0) {
     fprintf(out, "%s (%zu.%zu.%zu) {%0d} %s\n",
             (*name).c_str(),
             symb->lloc.filenr,
             symb->lloc.linenr,
             symb->lloc.offset,
             static_cast<int>(symb->block_nr),
             string_list.c_str());
    }
    else {
     fprintf(out, "%s (%zu.%zu.%zu) {%0d} %s %0d\n",
             (*name).c_str(),
             symb->lloc.filenr,
             symb->lloc.linenr,
             symb->lloc.offset,
             static_cast<int>(symb->block_nr),
             string_list.c_str(),
             static_cast<int>(symb->sequence));
    }
   }
}

void set_attr(astree* node, symbol* symb) {
    switch (node->symbol) {
    case TOK_VOID: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::VOID)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::VOID)] = 1;
     break;
     }
    case TOK_INT: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::INT)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::INT)] = 1;
     break;
     }
    case TOK_STRING: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::STRING)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::STRING)] = 1;
     break;
     }
    case TOK_NULLPTR: {
     if (symb == nullptr) {
      node->attributes[static_cast<unsigned>(attr::NULLPTR_T)] = 1;
      node->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     else {
      symb->attributes[static_cast<unsigned>(attr::NULLPTR_T)] = 1;
      symb->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     break;
     }
    case TOK_STRUCT: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::STRUCT)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::STRUCT)] = 1;
     break;
     }
    case TOK_PTR: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::PTR)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::PTR)] = 1;
     break;
     }
    case TOK_FUNCTION: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::FUNCTION)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::FUNCTION)] = 1;
     break;
     }
    case TOK_ARRAY: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::ARRAY)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::ARRAY)] = 1;
     break;
     }
    case TOK_PARAM: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::PARAM)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::PARAM)] = 1;
     break;
     }
    case TOK_FIELD: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::FIELD)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::FIELD)] = 1;
     break;
     }
    case TOK_TYPE_ID: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::TYPEID)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::TYPEID)] = 1;
     break;
     }
    case TOK_CHARCON: {
     if (symb == nullptr) {
      node->attributes[static_cast<unsigned>(attr::INT)] = 1;
      node->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     else {
      symb->attributes[static_cast<unsigned>(attr::INT)] = 1;
      symb->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     break;
     }
    case TOK_INTCON: {
     if (symb == nullptr) {
      node->attributes[static_cast<unsigned>(attr::INT)] = 1;
      node->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     else {
      symb->attributes[static_cast<unsigned>(attr::INT)] = 1;
      symb->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     break;
     }
    case TOK_STRINGCON: {
     if (symb == nullptr) {
      node->attributes[static_cast<unsigned>(attr::STRING)] = 1;
      node->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     else {
      symb->attributes[static_cast<unsigned>(attr::STRING)] = 1;
      symb->attributes[static_cast<unsigned>(attr::CONST)] = 1;
      }
     break;
     }
    case TOK_VARDECL: {
     if (symb == nullptr)
      node->attributes[static_cast<unsigned>(attr::VARIABLE)] = 1;
     else
      symb->attributes[static_cast<unsigned>(attr::VARIABLE)] = 1;
     break;
     }
    case TOK_IDENT: {
     if (symb == nullptr) {
      node->attributes[static_cast<unsigned>(attr::LVAL)] = 1;
      node->attributes[static_cast<unsigned>(attr::VARIABLE)] = 1;
      }
     else {
      symb->attributes[static_cast<unsigned>(attr::LVAL)] = 1;
      symb->attributes[static_cast<unsigned>(attr::VARIABLE)] = 1;
      }
     break;
     }
   }
}
bool type_check (astree* node) {
 for (uint i = 0; i < node->children.size(); i++ ) {
   switch(node->children[i]->symbol) {
   case TOK_VOID: { 
        if (node->symbol == TOK_FUNCTION) {
          errprintf("cannot have void function");
          return false; } break;
         }
   case '=': {
         if ( node->children[i]->children[0]->symbol == TOK_NULLPTR) {
          errprintf("cannot have null operand"); 
          return false; } break;
         }
   case TOK_ARROW: {
        if (node->children[i]->children[0]->attributes[
           static_cast<unsigned>(attr::STRUCT)]  != 1) {
           errprintf("left oper of '->' has to be struct");
           return false; } break;
         }
   default: break;
   } 
 }
 return true;
}
