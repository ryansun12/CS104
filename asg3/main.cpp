// Ryan Sun
// ryjsun@ucsc.edu
// 1609724
// CS104 asg2

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
using namespace std;
#include "string_set.h"
#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"

constexpr size_t LINESIZE = 1024;
string cpp_name = "/usr/bin/cpp";
string cpp_command;
//copied from main.cpp
void cpp_popen (const char* filename) {
   cpp_command = cpp_name + " " + filename;
   int i = 0;
   yyin = popen (cpp_command.c_str(), "r");
   if (yyin == nullptr) {
      syserrprintf (cpp_command.c_str());
      exit (exec::exit_status);
   } else {
     // int tok = yylex(); // print to token file, replace with yyparse
       // while (tok) {
         //  if (tok == YYEOF) { return; }
         //  i++;
          // tok = yylex();
           //printf(i);
      i++;
      lexer::newfilename (filename);
   }
}
//copied from main.cpp
void cpp_pclose() {
   int pclose_rc = pclose (yyin);
   eprint_status (cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
}
//some code copied from main.cpp
const char* scan_opts (int argc, char** argv) {
   opterr = 0;
   exec::execname = argv[0];
   yy_flex_debug = 0;
   yydebug = 0;
   lexer::interactive = isatty (fileno (stdin))
                    and isatty (fileno (stdout));
   for(;;) {
      int opt = getopt (argc, argv, "@lyD:");
      if (opt == EOF) break;
      switch (opt) {
         case '@': set_debugflags (optarg);   break;
         case 'D': cpp_name = cpp_name + optarg; // + " -D"
                   printf("%s\n", cpp_name.c_str()); break;
         case 'l': yy_flex_debug = 1;         break;
         case 'y': yydebug = 1;               break;
         default:  errprintf ("option error (%c)\n", optopt); break;
      }
   }
   if (optind > argc) {
      errprintf ("Usage: %s [-ly] [filename]\n",
                 exec::execname.c_str());
      exit (exec::exit_status);
   }
   const char* filename = optind == argc ? "-" : argv[optind];
   string outName = filename;
   int i = outName.find('.');
   if (outName.substr(i+1) != "oc") {
      errprintf("bad file, not .oc", exec::execname.c_str());
      exit (exec::exit_status); 
   }
   return filename;
}
FILE* tokfile;
FILE* strfile;
FILE* astfile;
//new main function
int main (int argc, char** argv) {
  const char* fileO = scan_opts(argc, argv);
  string n1 = string(fileO);
  int i = n1.find(".");
  n1 = n1.substr(0,i);
  tokfile = fopen((n1 + ".tok").c_str(), "w");
  strfile = fopen((n1 + ".str").c_str(), "w");
  astfile = fopen((n1 + ".ast").c_str(), "w");
  cpp_popen(fileO);
  int x = yyparse();
  // for (int y = 0; y < x; y++) { printf("%s",y)};
  cpp_pclose();
  yylex_destroy();
  string_set::dump(strfile);
  if (x) { errprintf ("failure with parser \n");}
     else { astree::print (astfile, yyparse_astree, 0); 
            delete parser::root; }
  return exec::exit_status;
}

