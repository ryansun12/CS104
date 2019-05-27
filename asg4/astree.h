// $Id: astree.h,v 1.10 2016-10-06 16:42:35-07 - - $

#ifndef __ASTREE_H__
#define __ASTREE_H__
#include <string>
#include <bitset>
#include <vector>
#include "auxlib.h"

using namespace std;


struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};
enum class attr {
VOID, INT, NULLPTR_T, STRING, STRUCT, ARRAY, FUNCTION, VARIABLE,
FIELD, TYPEID, PARAM, LOCAL, LVAL, CONST, VREG, VADDR, PTR, BITSET_SIZE,
};
using attr_bitset = bitset<static_cast<unsigned>(attr::BITSET_SIZE)>;

struct astree {

   // Fields.
   int symbol;               // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node
   attr_bitset attributes;
   size_t block_nr;
   string* struct_name = nullptr;
   string vreg; 
     // Functions.
   astree (int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt(astree* child1, astree* child2 = nullptr);
   astree* adopt_sym (astree* child, int symbol);
   void dump_node (FILE*);
      void dump_tree (FILE*, int depth = 0); 
   static string attr_string(attr_bitset a,
       string* struct_name);
   static void dump (FILE* outfile, astree* tree);
   static void print (FILE* outfile, astree* tree, int depth = 0);
};

extern vector<astree*> strincon_list;
void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif

