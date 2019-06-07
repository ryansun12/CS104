#ifndef __EMITTER_H__
#define __EMITTER_H__
#include <string>
#include <vector>
#include <iostream>

using namespace std;

#include "astree.h"
#include "lyutils.h"
#include "auxlib.h"
void emit_oil(astree* node);
void emit_struct(astree*node);
void emit_function(astree*node);
void emit_vardecl(astree*node);
int binopstmt(astree* node);
#endif
