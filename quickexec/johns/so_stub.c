#include <stdio.h>
#include <string.h>
#include <dlfcn.h>


int main( int argc, char *argv[] ) {
    char module[1024];
    int (*loadedMain)( int argc, char *argv[] );
    char *error;
    int retVal = 0;
    void *handle;
    strcpy( module, argv[0] );
    strcat( module, ".so" );
    if ( !(handle  = dlopen( module, RTLD_LAZY ) ) )
	fputs( dlerror(), stderr ), exit( 1 );
    loadedMain = dlsym( handle, "main" );
    if ( ( error = dlerror() ) != NULL )
	fputs( error, stderr ), exit( 1 );
    retVal = (*loadedMain)( argc, argv );
    dlclose( handle );
    return retVal;
}

