#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <dlfcn.h>


#define PID_FILE    "/tmp/qinit1.pid"
#define CMD_FILE    "/tmp/qinit1.cmd"
#define RET_FILE    "/tmp/qinit1.ret"


static int librariesFinishedLoading = 0; 
static void **handles = NULL;
static int handleCount = 0;


static void savePid( void );
static int loadPid( void );
void saveRetVal( int retVal );
int loadRetVal( void );
static void saveCmd( int callingPid, int argc, char *argv[] );
static int execCmd( void );
static void loadLibraries( char *listFile );
static void signalHandler( int signal );
static void finishedHandler( int signal );
static void cleanUpHandler( int signal );
static void cleanUp( void );


int main( int argc, char *argv[] ) {
    FILE *fh;

    // See if there is already a server running
    if ( ( fh = fopen( PID_FILE, "r" ) ) != NULL ) {
	// There appears to already be a server running
	fclose( fh );
	// Install handler that tells us when the process created by the server has finished
	signal( SIGUSR2, finishedHandler );
	// Send our command to the server (saved to a file)
	saveCmd( getpid(), argc, argv );
	// Send a signal to the server to run our command 
	kill( loadPid(), SIGUSR1 );
	// Wait for the process created by the server to terminate
	for (;;)
	    sleep( 1 ); // Will eventually terminate when the SIGUSER2 signal is sent to us by the server
    }

    // Send the server to the background
    daemon( 1, 1 );

    // Save the process number of this process somewhere (in a file)
    savePid();

    // Wait for signal used to let us know when to fork to run commands
    signal( SIGUSR1, signalHandler );
    signal( SIGTERM, cleanUpHandler );
    signal( SIGINT,  cleanUpHandler );
    signal( SIGABRT, cleanUpHandler );
    signal( SIGQUIT, cleanUpHandler );

    // Iterate library list file and dynamically load at runtime
    // the library files from the list
    if ( argc > 1 )
	loadLibraries( argv[1] ); // Use a library list file specified from command line
    else
	loadLibraries( "library.lst" ); // Use the default library list file

    // Run the given command
    signalHandler( SIGUSR1 );
 
    // Keep waiting for signals infinitely that tell us to fork
    for (;;)
	wait( NULL ); // Wait for child processes to die or signals to be sent to us

    cleanUp();

    return 0;
}


void cleanUp( void ) {
    int i;

    // Close the shared libraries we opened
    for ( i = 0; i < handleCount; i++ )
	dlclose( handles[ i ] );
    free( handles );

    // Unlink the PID_FILE file
    remove( PID_FILE );
}


void savePid( void ) {
    int pid = getpid();
    FILE *fh = fopen( PID_FILE, "w");
    if ( !fh )
	fputs("error writing pid to file " PID_FILE, stderr), exit(1);
    fprintf( fh, "%i", pid );
    fclose( fh );
}


int loadPid( void ) {
    int pid;
    FILE *fh = fopen( PID_FILE, "r" );
    if ( !fh )
	fputs("error loading pid from file " PID_FILE, stderr), exit(1);
    fscanf( fh, "%i", &pid );
    fclose( fh );
    return pid;
}


void saveRetVal( int retVal ) {
    FILE *fh = fopen( RET_FILE, "w");
    if ( !fh )
	fputs("error writing retVal to file " RET_FILE, stderr), exit(1);
    fprintf( fh, "%i", retVal );
    fclose( fh );
}


int loadRetVal( void ) {
    int retVal;
    FILE *fh = fopen( RET_FILE, "r" );
    if ( !fh )
	fputs("error loading retVal from file " RET_FILE, stderr), exit(1);
    fscanf( fh, "%i", &retVal );
    fclose( fh );
    return retVal;
}


void saveCmd( int callingPid, int argc, char *argv[] ) {
    int i;
    FILE *fh = fopen( CMD_FILE, "w" );
    if ( !fh )
	fputs("error saving arg list to file " CMD_FILE, stderr), exit(1);
    fprintf( fh, "%i\n", callingPid );
    fprintf( fh, "%s.so", argv[0] );
    for ( i = 0; i < argc; i++ )
	fprintf( fh, "\n%s", argv[i] );
    fclose( fh ); 
}


int execCmd( void ) {
    int callingPid;
    char sharedObject[128];
    int argc;
    char argv[128][128];
    int (*childMain)( int argc, char *argv[] );
    char *error;
    void *handle;
    int retVal = 0;

    // Get the name of the file to dlopen and the arguments to pass (from a file)
    FILE *fh = fopen( CMD_FILE, "r" );
    fscanf( fh, "%i\n", &callingPid );
    fgets( sharedObject, 128, fh );
    sharedObject[strlen( sharedObject ) - 1] = '\0';	
    for ( argc = 0; fgets( argv[argc], 128, fh ); argc++)
	argv[argc][strlen( argv[argc] ) - 1] = '\0';	
    fclose( fh );

    // Open the shared object of what we want to execute
    handle = dlopen( sharedObject, RTLD_LAZY );
    if ( !handle )
	fputs( dlerror(), stderr ), kill( callingPid, SIGUSR2 ), exit( 1 );

    // Find main symbol and jump to it
    childMain = dlsym( handle, "main" );
    if ( ( error = dlerror() ) != NULL )
	fputs( error, stderr ), kill( callingPid, SIGUSR2 ), exit( 1 );
    retVal = (*childMain)( argc, (char**)argv );

    // Notify caller that we have finished
    saveRetVal( retVal );
    kill( callingPid, SIGUSR2 );

    // When we are done, close the shared object
    dlclose( handle );
    return retVal;
}


void finishedHandler( int signal ) {
    // We have been notified of the completion of the task
    exit( loadRetVal() );
}


void cleanUpHandler( int signal ) {
    // Cleanup and exit if we have been told to quit
    cleanUp();
    exit(1);
}


void signalHandler( int signal ) {
    //printf("got a signal of %i\n", signal );

    if ( fork() == 0 ) {
	// We are the child process

/*
	while ( librariesFinishedLoading != 1 ) {
	    printf("waiting for libraries to load\n");
	    sleep(1);
	}
*/
	// Close file handles and open others etc

	// Execute the command and return when done
	exit( execCmd() );
    }
 
    // Parent process, just continue
    sleep( 1 ); // Need to yield to child process
}


void loadLibraries( char *listFile ) {
    FILE *fh = fopen( listFile, "r" );
    char line[1024];
    if ( !fh )
	fputs("error opening library list file", stderr), exit(1);
    while ( fgets( line, 1024, fh ) ) {
	line[strlen( line ) - 1] = '\0';
	printf("loading library: --%s--\n", line);
	handleCount++;
	handles = realloc( handles, handleCount * sizeof( void * ) );
	handles[ handleCount - 1 ] = dlopen( line, RTLD_NOW );
	if ( !handles[ handleCount - 1 ] )
	    fputs( dlerror(), stderr ), exit( 1 );
    }
    fclose( fh );
    librariesFinishedLoading = 1;
}

