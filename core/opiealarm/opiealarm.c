/*
 * opiealarm.c
 *
 *  This program is for extracting the event time/date out
 *  of /etc/resumeat and setting the RTC alarm to that time/date.
 *  It is designed to run via a script just before the iPAQ
 *  is suspended and right after the iPAQ resumes operation.
 * 
 *  written and copyrighted by Robert Griebl <sandman@handhelds.org>
 */

#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


#define PIDFILE 	"/var/run/opiealarm.pid"
#define APMFILE		"/proc/apm"

int resume ( int resuspend );
int suspend ( int fix_rtc );
int main ( int argc, char **argv );
int fork_with_pidfile ( void );
int kill_with_pidfile ( void );
void remove_pidfile ( void );
void usage ( void );
void sig_handler_child ( int sig );
void sig_handler_parent ( int sig );
int onac ( void );

static int opiealarm_was_running;
static pid_t parent_pid = 0;



void sig_handler_child ( int sig )
{
	// child got SIGUSR2 -> cleanup pidfile and exit

	remove_pidfile ( );
	exit ( 0 );
}

void sig_handler_parent ( int sig )
{
	// parent got SIGUSR1 -> safe to exit now

	parent_pid = 0;	
	exit ( 0 );
}

void usage ( void )
{
	fprintf ( stderr, "Usage: opiealarm -s [-f] | -r [-a]\n\n" );
	fprintf ( stderr, "\t-s\tSuspend mode: set RTC alarm\n" );
	fprintf ( stderr, "\t-f    \tFix RTC, if RTC and system have more than 5sec difference (suspend mode)\n" );
	fprintf ( stderr, "\t-r\tResume mode: kill running opiealarm\n" );
	fprintf ( stderr, "\t-a <x>\tResuspend in <x> seconds (resume mode)\n\n" );
	exit ( 1 );
}

int fork_with_pidfile ( void )
{
	FILE *fp;
	pid_t pid;
	
	pid = fork ( );
	
	if ( pid > 0 ) {
		// We can not just exit now, because the kernel could suspend 
		// the iPAQ just before the child process sets the RTC.
		// Solution: just wait for SIGUSR1 - the child process will 
		// signal this when it thinks it is safe to exit.
		
		signal ( SIGUSR1, sig_handler_parent );
		while ( 1 )
			sleep ( 1000 );
		exit ( 0 );
	}
	else if ( pid < 0 ) {
		perror ( "forking failed" );
		return 0;
	}

	// child process needs to react to SIGUSR2. This is sent when
	// a new opiealarm process is started.

	signal ( SIGUSR2, sig_handler_child );

	// save pid
	if (( fp = fopen ( PIDFILE, "w" ))) {
		fprintf ( fp, "%d", getpid ( ));
		fclose ( fp );

		// detach
		close ( 0 );
		close ( 1 );
		close ( 2 );

		setpgid ( 0, 0 );

		return 1;
	}
	else {
		perror ( PIDFILE );
		return 0;
	}
}

int kill_with_pidfile ( void )
{
	FILE *fp;
	pid_t pid;
	int res = 0;

	// terminate a running opiealarm child process
	// return 1 if we really killed one

	if (( fp = fopen ( PIDFILE, "r" ))) {
		if ( fscanf ( fp, "%d", &pid ) == 1 )
			res = ( kill ( pid, SIGUSR2 ) == 0 ) ? 1 : 0;
		fclose ( fp );
	}
	return res;
}

void remove_pidfile ( void )
{
	// child is about to exit - cleanup

	unlink ( PIDFILE );	
	signal ( SIGUSR2, SIG_DFL );
}


int main ( int argc, char **argv ) 
{
	int mode = 0;
	int ac_resusp = 0;
	int fix_rtc = 0;
	int opt;

	while (( opt = getopt ( argc, argv, "a:frs" )) != EOF ) {
		switch ( opt ) {
			case 's':
				mode = 's';
				break;
			case 'r':
				mode = 'r';
				break;
			case 'a':
				ac_resusp = atoi ( optarg );
				if ( ac_resusp < 30 ) {
					ac_resusp = 120;
					
					fprintf ( stderr, "Warning: resuspend timeout must be >= 30 sec. -- now set to 120 sec\n" );
				}
				break;
			case 'f':
				fix_rtc = 1;
				break;				
			default:
				usage ( );
		}
	}	
	
	if ( geteuid ( ) != 0 ) {
		fprintf ( stderr, "You need root priviledges to run opiealarm." );
		return 2;
	}
	
	if ( !mode )
		usage ( );


	parent_pid = getpid ( );

	// kill running opiealarm
	opiealarm_was_running = kill_with_pidfile ( );
	remove_pidfile ( );

	switch ( mode ) {
		case 'r': opt = resume ( ac_resusp );
		          break;
		case 's':
		default : opt = suspend ( fix_rtc );
		          break;
	}
	
	parent_pid = 0;
	return opt;
}		
		

int suspend ( int fix_rtc )
{
	FILE *fp;
	char buf [64];
	time_t alrt, syst, rtct;
	struct tm alr, sys, rtc;
	int fd;
	int rtc_sys_diff;
	
	
	if ( !fork_with_pidfile ( ))
		return 3;

	// we are the child process from here on ...

	tzset ( );  // not sure if it is really needed -- it probably doesn't hurt ...
	
	do { // try/catch simulation
	
		// read the wakeup time from /etc/resumeat
		if (!( fp = fopen ( "/etc/resumeat", "r" ))) 
			break; //  ( 1, "/etc/resumeat" );
	
		if ( !fgets ( buf, sizeof( buf ) - 1, fp ))
			break; //  ( 1, "/etc/resumeat" );

		fclose ( fp );
	
		alrt = atoi ( buf ); // get the alarm time
		if ( alrt == 0 )
			break; //  ( 0, "/etc/resumeat contains an invalid time description" );	
		alrt -= 5; 	// wake up 5 sec before the specified time	
		alr = *gmtime ( &alrt );

		time ( &syst );// get the UNIX system time
		sys = *localtime ( &syst );		
		
		if (( fd = open ( "/dev/misc/rtc", O_RDWR )) < 0 )
			if (( fd = open ( "/dev/rtc", O_RDWR )) < 0 )
				break; //  ( 1, "rtc" );	
		
		memset ( &rtc, 0, sizeof ( struct tm ));    // get the RTC time
		if ( ioctl ( fd, RTC_RD_TIME, &rtc ) < 0 )
			break; //  ( 1, "ioctl RTC_RD_TIME" );		
		rtct = mktime ( &rtc );

		rtc_sys_diff = ( syst - rtct ) - sys. tm_gmtoff;  // calculate the difference between system and hardware time
	
		if ( fix_rtc && (( rtc_sys_diff < -3 ) || ( rtc_sys_diff > 3 ))) {
			struct tm set;		
			set = *gmtime ( &syst );
			
			// if the difference between system and hardware time is more than 3 seconds,
			// we have to set the RTC (hwclock --systohc), or alarms won't work reliably.
	
	    	if ( ioctl ( fd, RTC_SET_TIME, &set ) < 0 )
		       break; //  ( 1, "ioctl RTC_SET_TIME" );
		}

		if ( ioctl ( fd, RTC_ALM_SET, &alr ) < 0 )  // set RTC alarm time
			break; //  ( 1, "ioctl RTC_ALM_SET" );		
 		if ( ioctl ( fd, RTC_AIE_ON, 0 ) < 0 )
			break; //  ( 1, "ioctl RTC_AIE_ON" );   // enable RTC alarm irq

		// tell the parent it is safe to exit now .. we have set the RTC alarm
		kill ( parent_pid, SIGUSR1 );
			
		if ( read ( fd, buf, sizeof( unsigned long )) < 0 ) // wait for the RTC alarm irq
			break; //  ( 1, "read rtc alarm" );
	
		// iPAQ woke up via RTC irq -- otherwise we would have received a SIGUSR2
		// from the "resume instance" of opiealarm.
	
		if ( ioctl ( fd, RTC_AIE_OFF, 0 ) < 0 )     // disable RTC alarm irq
			break; //  ( 1, "ioctl RTC_AIE_OFF" );
		
		close ( fd );
	
		remove_pidfile ( ); // normal exit
		return 0;	
		
	} while ( 0 );
	
	kill ( parent_pid, SIGUSR1 );  // parent is still running - it can exit now

	while ( 1 )		    // pretend that we are waiting on RTC, so opiealarm -r can kill us
		sleep ( 1000 ); // if we don't do this, the "resuspend on AC" would be triggerd
	return 0;
}


int onac ( void )
{
	FILE *fp;
	int on = 0;

	// check the apm proc interface for AC status

	if (( fp = fopen ( APMFILE, "r" ))) { 
		int ac = 0;
	
		if ( fscanf ( fp, "%*[^ ] %*d.%*d 0x%*x 0x%x 0x%*x 0x%*x %*d%% %*i %*c", &ac ) == 1 )
			on = ( ac == 0x01 ) ? 1 : 0;

		fclose ( fp );
	}
	return on;
}

int resume ( int resuspend )
{
	FILE *fp;

	// re-suspend when on AC (optional) when woken up via RTC

	if ( !opiealarm_was_running ) { 
		// if opiealarm -s didn't wake up via RTC, the old process gets killed
		// by kill_by_pidfile(), which is recorded in opiealarm_was_running
	
		if ( resuspend && onac ( )) {	
			time_t start, now;
			char *argv [4];
						
			if ( !fork_with_pidfile ( ))
				return 4;
				
			// we can't wait for the resuspend timeout in the parent process.
			// so we fork and tell the parent it can exit immediatly
				
			kill ( parent_pid, SIGUSR1 );

			// sleep <resuspend> seconds - this method is much more precise than sleep() !
			time ( &start );
			do {
				sleep ( 1 );
				time ( &now );
			} while (( now - start ) < resuspend );

			if ( onac ( )) { // still on ac ?
				argv[0] = "qcop";
				argv[1] = "QPE/Desktop";
				argv[2] = "suspend()";
				argv[3] = 0;
	
				// hard coded for now ...but needed
				// another way would be to simulate a power-button press
				
				setenv ( "LOGNAME", "root", 1 );
				setenv ( "HOME", "/root", 1 );
				setenv ( "LD_LIBRARY_PATH", "/opt/QtPalmtop/lib", 1 );
				setenv ( "QTDIR", "/opt/QtPalmtop", 1 );
			
				remove_pidfile ( );
			
				// no need for system() since this process is no longer usefull anyway				
				execv ( "/opt/QtPalmtop/bin/qcop", argv );
				
				perror ( "exec for qcop failed" );
				return 5;
			}
		}
	}
	return 0;
}
