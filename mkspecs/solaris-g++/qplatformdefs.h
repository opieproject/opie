#ifndef QPLATFORMDEFS_H
#define QPLATFORMDEFS_H

// Get Qt defines/settings

#include "qglobal.h"

// Set any POSIX/XOPEN defines at the top of this file to turn on specific APIs
#ifndef _POSIX_PTHREAD_SEMANTICS
#define _POSIX_PTHREAD_SEMANTICS
#endif

#include <unistd.h>


// We are hot - unistd.h should have turned on the specific APIs we requested


#ifdef QT_THREAD_SUPPORT
#include <pthread.h>
#endif

#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

// DNS header files are not fully covered by X/Open specifications.
// In particular nothing is said about res_* :/
// On Solaris header files <netinet/in.h> and <arpa/nameser.h> are not
// included by <resolv.h>. Note that <arpa/nameser.h> must be included
// before <resolv.h>.
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>


#if !defined(QT_NO_COMPAT)
#define QT_STATBUF		struct stat
#define QT_STATBUF4TSTAT	struct stat
#define QT_STAT			::stat
#define QT_FSTAT		::fstat
#define QT_STAT_REG		S_IFREG
#define QT_STAT_DIR		S_IFDIR
#define QT_STAT_MASK		S_IFMT
#define QT_STAT_LNK		S_IFLNK
#define QT_FILENO		fileno
#define QT_OPEN			::open
#define QT_CLOSE		::close
#define QT_LSEEK		::lseek
#define QT_READ			::read
#define QT_WRITE		::write
#define QT_ACCESS		::access
#define QT_GETCWD		::getcwd
#define QT_CHDIR		::chdir
#define QT_MKDIR		::mkdir
#define QT_RMDIR		::rmdir
#define QT_OPEN_RDONLY		O_RDONLY
#define QT_OPEN_WRONLY		O_WRONLY
#define QT_OPEN_RDWR		O_RDWR
#define QT_OPEN_CREAT		O_CREAT
#define QT_OPEN_TRUNC		O_TRUNC
#define QT_OPEN_APPEND		O_APPEND
#endif

#define QT_SIGNAL_RETTYPE	void
#define QT_SIGNAL_ARGS		int
#define QT_SIGNAL_IGNORE	SIG_IGN

#if !defined(_XOPEN_UNIX)
// Function usleep() is defined in C library but not declared in header files
// on Solaris 2.5.1. Not really a surprise, usleep() is specified by XPG4v2
// and XPG4v2 is only supported by Solaris 2.6 and better.
// Function gethostname() is defined in C library but not declared in <unistd.h>
// on Solaris 2.5.1.
// So we are trying to detect Solaris 2.5.1 using macro _XOPEN_UNIX which is
// not defined by <unistd.h> when XPG4v2 is not supported.
typedef unsigned int useconds_t;
extern "C" int usleep(useconds_t);
extern "C" int gethostname(char *, int);
#endif

#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE-0 >= 500) && (_XOPEN_VERSION-0 >= 500)
// on Solaris 7 and better with specific feature test macros
#define QT_SOCKLEN_T		socklen_t
#elif defined(_XOPEN_SOURCE_EXTENDED) && defined(_XOPEN_UNIX)
// on Solaris 2.6 and better with specific feature test macros
#define QT_SOCKLEN_T		size_t
#else
// always this case in practice
#define QT_SOCKLEN_T		int
#endif

#if defined(_XOPEN_UNIX)
// Supported by Solaris 2.6 and better.  XPG4v2 and XPG4v2 is also supported
// by Solaris 2.6 and better.  So we are trying to detect Solaris 2.6 using
// macro _XOPEN_UNIX which is not defined by <unistd.h> when XPG4v2 is not
// supported.
#define QT_SNPRINTF		::snprintf
#define QT_VSNPRINTF		::vsnprintf
#endif


#endif // QPLATFORMDEFS_H
