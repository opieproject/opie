/* opiealarm.c
*  This program is for extracting the event time/date out
*  of /etc/resumeat and setting the RTC alarm to that time/date.
*  It is designed to run via a script just before the iPaq
*  is suspended.
* 
*  Roughly based on ipaqalarm from Benjamin Long
*
*  written by Robert Griebl <sandman@handhelds.org>
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


#define PIDFILE 	"/var/run/opiealarm.pid"

FILE *log;


void log_msg ( const char *msg )
{
	if ( log ) {
		fprintf ( log, msg );
		fflush ( log );
	}
}

void error_msg_and_die ( int perr, const char *msg )
{
	if ( perr )
		log_msg ( strerror ( errno ));
	log_msg ( msg );

	unlink ( PIDFILE );
	exit ( 1 );
}



void suspend_on_rtc ( ) 
{
	FILE *fp;
	char buf [64];
	time_t t;
	struct tm *tm;
	int fd;
	

	if (!( fp = fopen ( "/etc/resumeat", "r" ))) 
		error_msg_and_die ( 1, "/etc/resumeat" );
	
	if ( !fgets ( buf, sizeof( buf ) - 1, fp ))
		error_msg_and_die ( 1, "/etc/resumeat" );

	fclose ( fp );
	
	t = atoi ( buf );
	
	if ( t == 0 )
		error_msg_and_die ( 0, "/etc/resumeat contains an invalid time description" );
	
	/* subtract 5 sec from event time... */
	t -= 5;

	if ( log )	
		fprintf ( log, "Setting RTC alarm to %d\n", t );
	
	tm = gmtime ( &t );

	// Write alarm time to RTC 
	if (( fd = open ( "/dev/misc/rtc", O_RDWR ) < 0 );
		error_msg_and_die ( 1, "/dev/misc/rtc" );	
	// set alarm time
	if ( ioctl ( fd, RTC_ALM_SET, tm ) < 0 )
		error_msg_and_die ( 1, "ioctl RTC_ALM_SET" );		
	// enable alarm irq	
	if ( ioctl ( fd, RTC_AIE_ON, 0 ) < 0 )
		error_msg_and_die ( 1, "ioctl RTC_AIE_ON" );
	
	log_msg ( "SLEEPING\n" );
			
	// wait for alarm irq
	if ( read ( fd, buf, sizeof( unsigned long )) < 0 )
		error_msg_and_die ( 1, "read rtc alarm" );
	
	log_msg ( "WAKEUP\n" );
	
	// disable alarm irq
	if ( ioctl ( fd, RTC_AIE_OFF, 0 ) < 0 ) 
		error_msg_and_die ( 1, "ioctl RTC_AIE_OFF" );
		
	close ( fd );
}


void sig_handler ( int sig )
{
	log_msg ( "GOT SIGNAL -> EXITING\n" );
	fclose ( log );
	unlink ( PIDFILE );
	exit ( 0 );
}


int main ( ) 
{
	FILE *fp;
	pid_t pid;

	if ( geteuid ( ) != 0 ) {
		fprintf ( stderr, "You need root priviledges to run opiealarm." );
		return 1;
	}
	
	pid = fork ( );
	
	if ( pid > 0 )
		return 0;
	else if ( pid < 0 ) {
		perror ( "Could not fork." );
		return 2;
	}
	
	// save pid
	if (!( fp = fopen ( PIDFILE, "w" ))) {
		perror ( PIDFILE );
		return 3;
	}
	
	fprintf ( fp, "%d", getpid ( ));
	fclose ( fp );

	// detach
	close ( 0 );
	close ( 1 );
	close ( 2 );

	setpgid ( 0, 0 );

	log = fopen ( "/tmp/opiealarm.log", "w" ); 
	log_msg ( "STARTING\n" );
	
	signal ( SIGTERM, sig_handler );
	signal ( SIGINT,  sig_handler );

	extractevent ( );
	
	signal ( SIGTERM, SIG_DFL );
	signal ( SIGINT,  SIG_DFL );

	log_msg ( "EXITING\n" );
	
	fclose ( log );
	unlink ( PIDFILE );
	return 0;
}
