// Ryan Sun
// ryjsun@ucsc.edu
// 1609724
// CS104 asg1

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

constexpr size_t LINESIZE = 1024;
string CPP = "/usr/bin/cpp -nostdinc";
//copied from cppstrtok.cpp, chomp last char from buffer if it is delim
void chomp(char* string, char delim){
   size_t len = strlen(string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// run cpp against lines of the file copied from cppstrtok.cpp
void cpplines (FILE* pipe, const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy(inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      const char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == nullptr) break;
      chomp (buffer, '\n');
     // printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
     // char inputname[LINESIZE];
      sscanf(buffer, "# %d \"%[^\"]\"", &linenr, inputname);
      char* savepos = nullptr;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = nullptr;
         if (token == nullptr) break;
         string_set::intern(token);
      }
      ++linenr;
   }
}

int main (int argc, char** argv) {
//deal with flags
  // int yydebug = 0, yy_flex_debug = 0;
   int opt = getopt(argc, argv, "ly@D:");
   while (opt != -1){
      switch (opt) {
         case '@' :// set_debugflags(optarg);
                    break;
         case 'D' :// CPP = CPP + " " + optarg;
                    break;
         case 'l' :// yy_flex_debug = 1; 
                    break;
         case 'y' :// yydebug = 1; 
                    break;
         default : fprintf(stderr, "error\n"); break;
       }
   }
   string temp = basename(argv[optind]);
   // parts from cppstrtok.cpp
   const char* execname = basename(argv[0]);
   int exit_status = EXIT_SUCCESS; 
      string filename = argv[argc - 1];
      string command = CPP + " " + filename;
   int i = filename.find('.'); //check oc file
   if (filename.substr(i, filename.length()) != ".oc"){
      fprintf(stderr, "not an .oc file");
   } 
   string name = temp.substr(0,temp.size() -3) + ".str";  
     // const string* str = string_set::intern (argv[i]);
     // printf ("intern (\"%s\") returned %p->\"%s\"\n",
     //         argv[i], str, str->c_str());
   FILE* pipe = popen (command.c_str(), "r");
   if (pipe == nullptr) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
               execname, command.c_str(), strerror (errno));
   } else {
      cpplines (pipe, filename.c_str());
      int pclose_rc = pclose (pipe);
      eprint_status (command.c_str(), pclose_rc);
      if (pclose_rc != 0){ exit_status = EXIT_FAILURE;}
   }
   const char* out = name.c_str();
   FILE* output = fopen(out,"w");
   string_set::dump(output);
   fclose(output);
   return exit_status;
}

