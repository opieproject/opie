#ifndef QUICKEXEC_H
#define QUICKEXEC_H

#define HAVE_QUICKEXEC
/*
  A special version of execl that opens a shared library and executes the main() function in
  the lib. Requires the quickexec daemon to run.
  
  Note: You do not need to call fork() before calling quickexec, the function actually returns.

  WARNING: path has to be a shared library, otherwise quickexec will hang 
*/

extern int quickexecl( const char *path, const char *, ...);
extern int quickexecv( const char *path, const char *argv[] );

#endif
