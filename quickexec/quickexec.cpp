#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>

#define QUICKEXEC "/tmp/.quickexec"

#include <signal.h>
#include <sys/wait.h>

void sigchildhandler(int) {
    wait(0);
}

int exec( const char *lib, int argc, char** argv )
{
    setpgid(0,0);
// printf("loadlib %s, argc=%d\n", lib, argc );
    void *handle = dlopen ( lib, RTLD_LAZY);
    if (!handle) {
	fprintf( stderr, "%s\n", dlerror());
	exit(1);
    }
    typedef int (*Mainfunc)(int, char**);
    Mainfunc mainfunc;
    mainfunc = (Mainfunc) dlsym(handle, "main");
    char *error;
    if ((error = dlerror()) != NULL)  {
	fprintf (stderr, "%s\n", error);
	exit(1);
    }
    (*mainfunc)(argc,argv);
    return 0;
}

#define BUFFER_SIZE 1024
int main( int argc, char** argv )
{
    signal( SIGCHLD, sigchildhandler );
    (void) unlink( QUICKEXEC );
    if ( mkfifo( QUICKEXEC, S_IFIFO | S_IWUSR | S_IRUSR ) == -1 ) {
	perror( QUICKEXEC );
	exit(1);
    }
    
    if ( argc > 1 && fork() == 0 ) 
	return exec( argv[1], argc-2, argc > 2 ? argv+2 : 0 );
    
    char buf[BUFFER_SIZE];
    int p = 0;
    int r;
    int fd = open( QUICKEXEC, O_RDONLY );
    if ( fd == -1 ) {
	perror( QUICKEXEC );
	exit(1);
    }
    for ( ;; ) {
	r = read( fd, buf+p, BUFFER_SIZE-p );
	p += r;
	if ( r == 0 || p >= BUFFER_SIZE - 1 ) {
	    buf[p] = '\0';
	    close ( fd );
	    if ( fork() == 0 ) {
		int argc = -1;
		int i = 0;
		int k = 0;
		for ( i = 0; i <= p; i++ )
		    if ( buf[i] == '\0' )
			argc++;
		char** argv = new char*[argc];
		for ( i = 0; i < p; i++ ) {
		    if ( buf[i] == '\0' )
			argv[k++]=buf+i+1;
		}
		return exec( buf, argc, argv );
	    }
	    p = 0;
	    fd = open( QUICKEXEC, O_RDONLY );
	}
    }
    return 0;
}














