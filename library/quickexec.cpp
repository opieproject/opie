#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define QUICKEXEC "/tmp/.quickexec"

int quickexecv( const char *path, const char *argv[] )
{
    int fd = open( QUICKEXEC, O_WRONLY );
    if ( fd == -1 ) {
	perror( "quickexec pipe" );
	return -1;
    }
    write( fd, path, strlen( path )+1 );
    const char **s = argv;
    while( *s ) {
	write( fd, *s, strlen( *s )+1 );
	++s;
    }
    close(fd);
    return 0;
}

int quickexec( const char *path, const char *, ...)
{
    int fd = open( QUICKEXEC, O_WRONLY );
    if ( fd == -1 ) {
	perror( "quickexec pipe" );
	return -1;
    }
    const char** s = &path;
    do {
	write( fd, *s, strlen( *s )+1 );
    } while ( *(++s) );

    close( fd );

    return 0;
}
