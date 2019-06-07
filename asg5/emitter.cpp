// Ryan Sun
// ryjsun@ucsc.edu
// Assignment 5 
// Intermediate Language Emitter

#include <string>
#include "emitter.h"
#include "lyutils.h"
#define spaces "          "

using namespace std;
extern FILE* oilfile;
int stringcount = 0;
int regcount = 0;
int count = 0;

void stringDFS(astree* node){
 for(uint i =0; i < node->children.size(); i++){
   if(node->children[i]->symbol == TOK_STRINGCON) {
   string* str = const_cast<string*>(node->children[i]->lexinfo);
   fprintf(oilfile,".s%d:       %s\n",stringcount, (*str).c_str());
   stringcount++;
   }
   if(node->children[i]->children.size() >= 1) {
      stringDFS(node->children[i]);
   }
 }
}

void emit_oil(astree* node){
   stringDFS(node);
// printf("DEBUG>>> post string search");
   fprintf(oilfile, "\n");
   for(uint i = 0; i < node->children.size(); i++) {
     switch(node->children[i] -> symbol) {
      case TOK_STRUCT: emit_struct(node->children[i]); break;
      case TOK_FUNCTION: {
           count = 0;
           emit_function(node->children[i]); 
           break; }
//      case TOK_VARDECL:
      case TOK_TYPE_ID: emit_vardecl(node->children[i]); break;
      default: break;
    }
   } 
}

void emit_struct(astree* node) {
   string* name = const_cast<string*>(node->children[0]->lexinfo);
   fprintf(oilfile, "%s.struct %s \n",spaces,(*name).c_str());
   if(node->children.size() > 1){
     for(uint i = 1; i < node->children.size(); i++) {
     astree* child = node->children[i];
     string* cname = const_cast<string*>(child->children[1]->lexinfo);
     string* ctype = const_cast<string*>(child->children[0]->lexinfo);
     fprintf(oilfile, "%s.field %s %s \n", spaces,
       (*ctype).c_str(), (*cname).c_str());
     }
   }
  fprintf(oilfile, "%s.end \n\n", spaces); 
}

void emit_function(astree* node) { 
   string* name = const_cast<string*>
     (node->children[0]->children[1]->lexinfo);
   string* type = const_cast<string*>
     (node->children[0]->children[0]->lexinfo);
   fprintf(oilfile, "%s: ",(*name).c_str());
   if ((*name).size() >= 10) {
      fprintf(oilfile, " ");
   }
   else {
      for(uint k = 2; k < 10 - (*name).size(); k++) {
         fprintf(oilfile, " ");
      }
   }
   if (node->children[0]->children[0]->symbol == TOK_VOID){
      fprintf(oilfile, ".function \n");
   }
   else {
      fprintf(oilfile, ".function %s \n", (*type).c_str());
   }

  //params
   for(uint p = 0; p < node->children[1]->children.size(); p++) {   
     string* pname = const_cast<string*>
     (node->children[1]->children[p]->children[1]->lexinfo);
     string* ptype = const_cast<string*>
     (node->children[1]->children[p]->children[0]->lexinfo);
     fprintf(oilfile, "%s.param %s %s \n", spaces,
     (*ptype).c_str(), (*pname).c_str());
   }
  //block
   if (node->children.size() == 3){
      for(uint i = 0; i < node->children[2]->children.size(); i++) {
       switch(node->children[2]->children[i]->symbol) { 
        case TOK_TYPE_ID: {
     string* tname = const_cast<string*>
     (node->children[2]->children[i]->children[1]->lexinfo);
     string* ttype = const_cast<string*>
     (node->children[2]->children[i]->children[0]->lexinfo);
     string* tval = const_cast<string*>
     (node->children[2]->children[i]->children[2]->lexinfo);
     //fix: if alloc or ptr 
     fprintf(oilfile, "%s.local %s %s \n",spaces, (*ttype).c_str(),
     (*tname).c_str());
     fprintf(oilfile, "%s%s = %s\n", spaces, (*tname).c_str(),
     (*tval).c_str());
     break;
        }
        case TOK_WHILE: { 
     fprintf(oilfile, ".wh%d:     goto .od%d if ", count,count );
     astree* wchild0 = node->children[2]->children[i]->children[0];
     string op = "";
      switch(wchild0->symbol) {
       case TOK_LT: op += ">="; break;
       case TOK_GT: op += "<="; break;
       case TOK_GE: op += "<"; break;
       case TOK_LE: op += ">"; break;
       case TOK_EQ: op += "!="; break;
       case TOK_NE: op += "=="; break;
       default: break;
      }
   if(wchild0->symbol == TOK_INTCON || wchild0->symbol == TOK_IDENT) {
     string* op1 = const_cast<string*>(wchild0->lexinfo);
     fprintf(oilfile, "%s\n",(*op1).c_str() );
   }
   else {
     string* op1 = const_cast<string*>(wchild0->children[0]->lexinfo);
     string* op2 = const_cast<string*>(wchild0->children[1]->lexinfo);
     fprintf(oilfile, "%s %s %s\n",(*op1).c_str(),
     op.c_str(), (*op2).c_str() );
    }
     astree* wblk = node->children[2]->children[i]->children[1]; 
     for(uint j = 0; j < wblk->children.size(); j++) {
       switch(wblk->children[j]->symbol) {
        case '=': {
        if(j == wblk->children.size() - 1) { //increment while counter
         string* bop = const_cast<string*>
          (wblk->children[j]->children[1]->lexinfo);
         string* bop1 = const_cast<string*>
          (wblk->children[j]->children[1]->children[0]->lexinfo);
         string* bop2 = const_cast<string*>
         (wblk->children[j]->children[1]->children[1]->lexinfo);
         fprintf(oilfile, ".do%d:     $t%d:i =", count, regcount++);
         fprintf(oilfile," %s %s %s \n%sgoto .wh0\n",
         (*bop1).c_str(),(*bop).c_str(), (*bop2).c_str(), spaces);
        }
        else { //fix this
         astree* c1 = wblk->children[j]->children[1];
         if (binopstmt(c1) == 1) {             
           string* bop = const_cast<string*>(c1->lexinfo);
           string* bop1 = const_cast<string*>
           (wblk->children[j]->children[0]->lexinfo);
            string* bop2 = const_cast<string*>
           (c1->children[1]->lexinfo);
          fprintf(oilfile, ".do%d:     $t%d:i =", count, regcount++);
          fprintf(oilfile," %s %s %s \n", (*bop1).c_str(),
          (*bop).c_str(), (*bop2).c_str());
         }
        else {
          string* bop1 = const_cast<string*>
          (wblk->children[j]->children[0]->lexinfo);
         string* bop2 = const_cast<string*>
          (wblk->children[j]->children[1]->lexinfo);
         fprintf(oilfile, ".do%d:     $t%d:i =", count, regcount++);
         fprintf(oilfile," %s %s \n",(*bop1).c_str(), (*bop2).c_str());
          }
        }  
        break;
       } 
        case TOK_IF:{  
         astree* ifnode = wblk->children[j];
         if (ifnode->children.size() == 2) {
         string* iop = const_cast<string*>
          (ifnode->children[0]->lexinfo);
         string* iopc1 = const_cast<string*>
          (ifnode->children[0]->children[0]->lexinfo);
         string* iopc2 = const_cast<string*>
          (ifnode->children[0]->children[1]->lexinfo);
          fprintf(oilfile, ".if%d:     $t%d:i = ",count,regcount++);
          fprintf(oilfile, "%s %s %s\n", (*iopc1).c_str(),
           (*iop).c_str(), (*iopc2).c_str());
         string* istmt = const_cast<string*>(ifnode->children[1]->
           lexinfo);
          fprintf(oilfile, ".th%d:     $t%d:i = ",count, regcount++);
          fprintf(oilfile, "%s\n", (*istmt).c_str());
         }
         else {
          string* iop = const_cast<string*>
          (ifnode->children[0]->lexinfo);
          string* iopc1 = const_cast<string*>
          (ifnode->children[0]->children[0]->lexinfo);
          string* iopc2 = const_cast<string*>
          (ifnode->children[0]->children[1]->lexinfo);
          fprintf(oilfile, ".if%d:     $t%d:i = ",count,regcount);
          fprintf(oilfile, "%s %s %s\n",
          (*iopc1).c_str(), (*iop).c_str(), (*iopc2).c_str());
         string* istmt = const_cast<string*>(ifnode->children[1]->
           lexinfo);
          fprintf(oilfile, ".th%d:     $t%d:i = ",count, regcount++);
          fprintf(oilfile, "%s\n", (*istmt).c_str());
          fprintf(oilfile, "%sgoto .fi%d\n",spaces,count);
         string* iexpr = const_cast<string*>(ifnode->children[2]->
           lexinfo);
          fprintf(oilfile, ".el%d:     $t%d:i = ",count, regcount++);
          fprintf(oilfile, "%s\n", (*iexpr).c_str()); 
           }
       break;}
        case TOK_CALL: {     //fix this 
        string* c1 = const_cast<string*>(wblk->children[j]->
          children[0]->lexinfo);
        string* c2 = const_cast<string*>(wblk->children[j]->
          children[1]->lexinfo);
        if (wblk->children[j]->children[1]->symbol 
           == TOK_STRINGCON) {       
         fprintf(oilfile, ".do%d:     %s(.s%d)\n",count,
         (*c1).c_str(),stringcount - 1);
        }
        else {
         fprintf(oilfile, ".do%d:     %s(%s)\n",count,
          (*c1).c_str(),(*c2).c_str());
         }
          break; 
         }
        default: break;
       }
     }
     fprintf(oilfile, ".od%d:\n", count);
     count++;
     break;
        } //end WHILE
        case TOK_IF: {
         astree* ifnode = node->children[2]->children[i];
         if (ifnode->children.size() == 2) {
         string* iop = const_cast<string*>
          (ifnode->children[0]->lexinfo);
         string* iopc1 = const_cast<string*>
          (ifnode->children[0]->children[0]->lexinfo);
         string* iopc2 = const_cast<string*>
          (ifnode->children[0]->children[1]->lexinfo);
          fprintf(oilfile, ".if%d:     $t%d:i = ",count,regcount++);
          fprintf(oilfile, "%s %s %s\n", (*iopc1).c_str(),
           (*iop).c_str(), (*iopc2).c_str());
         string* istmt = const_cast<string*>(ifnode->children[1]->
           lexinfo);
          fprintf(oilfile, ".th%d:     $t%d:i = ",count, regcount++);
          fprintf(oilfile, "%s\n", (*istmt).c_str());
         }
         else {
          string* iop = const_cast<string*>
          (ifnode->children[0]->lexinfo);
          string* iopc1 = const_cast<string*>
          (ifnode->children[0]->children[0]->lexinfo);
          string* iopc2 = const_cast<string*>
          (ifnode->children[0]->children[1]->lexinfo);
          fprintf(oilfile, ".if%d:     $t%d:i = ",count,regcount);
          fprintf(oilfile, "%s %s %s\n",
          (*iopc1).c_str(), (*iop).c_str(), (*iopc2).c_str());
         string* istmt = const_cast<string*>(ifnode->children[1]->
           lexinfo);
          fprintf(oilfile, ".th%d:     $t%d:i = ",count, regcount++);
          fprintf(oilfile, "%s\n", (*istmt).c_str());
          fprintf(oilfile, "%sgoto .fi%d\n",spaces,count);
         string* iexpr = const_cast<string*>(ifnode->children[2]->
           lexinfo);
          fprintf(oilfile, ".el%d:     $t%d:i = ",count, regcount++);
          fprintf(oilfile, "%s\n", (*iexpr).c_str());
          fprintf(oilfile, ".fi%d:\n",count);
         }
     break; }
        case TOK_CALL: { 
     string* c1 = const_cast<string*>
       (node->children[2]->children[i]->children[0]->lexinfo);
     string* c2 = const_cast<string*>
       (node->children[2]->children[i]->children[1]->lexinfo);
     if (node->children[2]->children[i]->children[1]->symbol 
           == TOK_STRINGCON) {       
       fprintf(oilfile, ".do%d:     %s(.s%d)\n",count,
       (*c1).c_str(),stringcount - 1); 
     }
     else {
     fprintf(oilfile, ".do%d:     %s(%s)\n",count,
       (*c1).c_str(),(*c2).c_str());
     }
     break; }
        case TOK_RETURN:{
        string* rtrn = const_cast<string*>(node->children[2]->
        children[i]->children[0]->lexinfo);
        fprintf(oilfile, "%sreturn %s\n", spaces,(*rtrn).c_str());
     break;}
        default: break;
       }
      } 
   }
//   if(node->children.size() > 1){
//     for(uint i = 1; i < node->children.size(); i++) {
 fprintf(oilfile, "%sreturn\n%s.end\n\n",spaces, spaces);
}
void emit_vardecl(astree* node) {
   string* type = const_cast<string*>(node->children[0]->lexinfo);
   string* name = const_cast<string*>(node->children[1]->lexinfo);
   string* value = const_cast<string*>(node->children[2]->lexinfo);
//    printf("%s\n", (*value).c_str());
//    printf("%s\n", (*name).c_str());
   if (node->children[0]->symbol == TOK_STRINGCON) {
      fprintf(oilfile, ".s%d:         %s", stringcount,
         (*value).c_str());
      stringcount++;
      return;
   }
//   if (node->children[0]->symbol == TOK_ARRAY) { 
//      fprintf(oilfile, "%s:", (*name).c_str());
//      return;
//   }
   fprintf(oilfile, "%s:", (*name).c_str());
   for(uint i = 1; i < 10 - (*name).size(); i++) {
      fprintf(oilfile, " ");
   }
   fprintf(oilfile, ".global %s %s \n\n",
    (*type).c_str(), (*value).c_str());
   return;
}

int binopstmt(astree* node) {
  switch (node->symbol) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '%': return 1;
  default : break;
  }
  return 0;
}
