#ifndef QPLATFORMDEFS_H
#define QPLATFORMDEFS_H

// Get Qt defines/settings

#include "qglobal.h"

// Set any POSIX/XOPEN defines at the top of this file to turn on specific APIs

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
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

// DNS header files are not fully covered by X/Open specifications.
// In particular nothing is said about res_* :/
// On BSDs header files <netinet/in.h> and <arpa/nameser.h> are not
// included by <resolv.h>. Note that <arpa/nameser.h> must be included
// before <resolv.h>.
#include <netinet/in.h>
#define class c_class // FreeeBSD 3.x
#include <arpa/nameser.h>
#undef class
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

#if __FreeBSD_version < 400000
// FreeBSD 1.0 - 3.5.1
#  define QT_SOCKLEN_T		int
#else
// FreeBSD 4.0 - 5.0
#  define QT_SOCKLEN_T		socklen_t
#endif

#define QT_SNPRINTF		::snprintf
#define QT_VSNPRINTF		::vsnprintf

// Older FreeBSD versions may still use the a.out format instead of ELF.
// From the FreeBSD man pages:
// 	In previous implementations, it was necessary to prepend an
// 	underscore to all external symbols in order to gain symbol
// 	compatibility with object code compiled from the C language.
// 	This is still the case when using the (obsolete) -aout option to
// 	the C language compiler.
#ifndef __ELF__
#define QT_AOUT_UNDERSCORE
#endif


#endif // QPLATFORMDEFS_H
