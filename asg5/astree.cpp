// $Id: astree.cpp,v 1.17 2019-03-15 14:32:40-07 - - $

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include "astree.h"
#include "string_set.h"
#include "lyutils.h"
#include "sym.h"
vector<astree*> stringcon_list;

astree::astree (int symbol_, const location& lloc_, const char* info) {
   symbol = symbol_;
   lloc = lloc_;
   lexinfo = string_set::intern (info);
   // vector defaults to empty -- no children
}

astree::~astree() {
   while (not children.empty()) {
      astree* child = children.back();
      children.pop_back();
      delete child;
   }
   if (yydebug) {
      fprintf (stderr, "Deleting astree (");
      astree::dump (stderr, this);
      fprintf (stderr, ")\n");
   }
}

astree* astree::adopt (astree* child1, astree* child2) {
   if (child1 != nullptr) children.push_back (child1);
   if (child2 != nullptr) children.push_back (child2);
   return this;
}

astree* astree::adopt_sym (astree* child, int symbol_) {
   symbol = symbol_;
   return adopt (child);
}
void astree::dump_node (FILE* outfile) {
   fprintf (outfile, "%p->{%s %zd.%zd.%zd \"%s\":",
            static_cast<const void*> (this),
            parser::get_tname (symbol),
            lloc.filenr, lloc.linenr, lloc.offset,
            lexinfo->c_str());
   for (size_t child = 0; child < children.size(); ++child) {
      fprintf (outfile, " %p",
               static_cast<const void*> (children.at(child)));
   }
}

void astree::dump_tree (FILE* outfile, int depth) {
   fprintf (outfile, "%*s", depth * 3, "");
   dump_node (outfile);
   fprintf (outfile, "\n");
   for (astree* child: children) child->dump_tree (outfile, depth + 1);
   fflush (nullptr);
}

void astree::dump (FILE* outfile, astree* tree) {
   if (tree == nullptr) fprintf (outfile, "nullptr");
                   else tree->dump_node (outfile);
}

void astree::print (FILE* outfile, astree* tree, int depth) 
{
   const char *tname = parser::get_tname (tree->symbol);
   if (strstr (tname, "TOK_") == tname) tname += 4;
   for(int i = 0; i < depth; i++){
       fprintf(outfile, "%s", "|  ");}
   string str = attr_string(tree->attributes,
         tree->struct_name);
  // fprintf (outfile, "; %*s", depth * 3, "");
   fprintf (outfile, "%s \"%s\" (%zd.%zd.%zd) %s \n",
      tname, tree->lexinfo->c_str(),tree->lloc.filenr,
      tree->lloc.linenr, tree->lloc.offset
      , str.c_str());
   for (astree* child: tree->children) {
      astree::print (outfile, child, depth + 1);
   }
}

void destroy (astree* tree1, astree* tree2) {
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
}

void errllocprintf (const location& lloc, const char* format,
                    const char* arg) {
   static char buffer[0x1000];
   assert (sizeof buffer > strlen (format) + strlen (arg));
   snprintf (buffer, sizeof buffer, format, arg);
   errprintf ("%s:%zd.%zd: %s", 
              lexer::filename (lloc.filenr), lloc.linenr, lloc.offset,
              buffer);
}
string astree::attr_string(attr_bitset a, 
     string* struct_name) {
//   printf("DEBUG>>> start AST table attribute...\n");
   string strList = "";
      if (a[static_cast<unsigned>(attr::STRUCT)] == 1) {
        strList += "struct " + *struct_name + " ";
    } if (a[static_cast<unsigned>(attr::PTR)] == 1) {
        strList += "ptr <" + *struct_name + "> ";
    } if (a[static_cast<unsigned>(attr::INT)] == 1) {
        strList += "int ";
    } if (a[static_cast<unsigned>(attr::STRING)] == 1) {
        strList += "string ";
    } if (a[static_cast<unsigned>(attr::VARIABLE)] == 1) {
        strList += "variable ";
    } if (a[static_cast<unsigned>(attr::NULLPTR_T)] == 1) {
        strList += "null ";
    } if (a[static_cast<unsigned>(attr::FUNCTION)] == 1) {
        strList += "function ";
    } if (a[static_cast<unsigned>(attr::LVAL)] == 1) {
        strList += "lval ";
    } if (a[static_cast<unsigned>(attr::PARAM)] == 1) {
        strList += "param ";
    } if (a[static_cast<unsigned>(attr::CONST)] == 1) {
        strList += "const ";
    } if (a[static_cast<unsigned>(attr::VREG)] == 1) {
        strList += "vreg ";
    } if (a[static_cast<unsigned>(attr::VADDR)] == 1) {
        strList += "vaddr ";
    } if (a[static_cast<unsigned>(attr::VOID)] == 1) {
        strList += "void ";
    } if (a[static_cast<unsigned>(attr::ARRAY)] == 1) {
        strList += "array ";
    } if (a[static_cast<unsigned>(attr::LOCAL)] == 1) {
        strList += "local ";
    }
      if (a[static_cast<unsigned>(attr::FIELD)] == 1) {
        strList += "field ";
    }
  //  printf("DEBUG>>> AST table attribute completed\n");
    return strList;
}
