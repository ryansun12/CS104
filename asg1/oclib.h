// $Id: oclib.h,v 1.31 2019-04-11 12:36:19-07 - - $

#ifndef __OCLIB_H__
#define __OCLIB_H__

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EOF (-1)
#define TRUE 1
#define FALSE 0

#define assert(expr) \
        if (not (expr)) assert_fail (#expr, __FILE__, __LINE__); 

void assert_fail (string expr, string file, int line);

void putchr (int c);
void putint (int i);
void putstr (string s);

int getchr();
string getstr();
string getln();

void exit (int status);

#endif

