/* aconf.h.  Generated automatically by configure.  */
/*
 * aconf.h
 *
 * Copyright 2002 Derek B. Noonburg
 */
 
/* 
 * Manually edited for QPDF (Robert Griebl)
 */

#ifndef ACONF_H
#define ACONF_H

// vv QPDF specific

#include "fixed.h"

//template <> class fixed<10>;

typedef fixed<10> fouble;

// ^^ QPDF specific


/*
 * Full path for the system-wide xpdfrc file.
 */
#define SYSTEM_XPDFRC "/etc/xpdfrc"

/*
 * Various include files and functions.
 */
#define HAVE_DIRENT_H 1
/* #undef HAVE_SYS_NDIR_H */
/* #undef HAVE_SYS_DIR_H */
/* #undef HAVE_NDIR_H */
/* #undef HAVE_SYS_SELECT_H */
/* #undef HAVE_SYS_BSDTYPES_H */
/* #undef HAVE_STRINGS_H */
/* #undef HAVE_BSTRING_H */
#define HAVE_POPEN 1
#define HAVE_MKSTEMP 1
/* #undef SELECT_TAKES_INT */

#endif
