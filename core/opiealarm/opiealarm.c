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
#include <errno.h>
#include <string.h>


#define PIDFILE 	"/var/run/opiealarm.pid"
#define APMFILE		"/proc/apm"

FILE *log; // debug only

int resume ( int resuspend );
int suspend ( int fix_rtc );
int main ( int argc, char **argv );
int fork_with_pidfile ( void );
int kill_with_pidfile ( void );
void remove_pidfile ( void );
void usage ( void );
void sig_handler ( int sig );
void error_msg_and_die ( int perr, const char *msg );
int onac ( void );

static int opiealarm_was_running;


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

	while ( 1 )		// pretend we are waiting on RTC, so opiealarm -r can kill us
		sleep ( 1 );
}


void sig_handler ( int sig )
{
	log_msg ( "GOT SIGNAL -> EXITING\n" );
	fclose ( log );
	remove_pidfile ( );
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
	
	if ( pid > 0 )
		exit ( 0 );
	else if ( pid < 0 ) {
		perror ( "forking failed" );
		return 0;
	}

	signal ( SIGTERM, sig_handler );
	signal ( SIGINT,  sig_handler );

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

	if (( fp = fopen ( PIDFILE, "r" ))) {
		if ( fscanf ( fp, "%d", &pid ) == 1 )
			return ( kill ( pid, SIGTERM ) == 0 ) ? 1 : 0;
		fclose ( fp );
	}
	return 0;
}

void remove_pidfile ( void )
{
	unlink ( PIDFILE );
	
	signal ( SIGTERM, SIG_DFL );
	signal ( SIGINT,  SIG_DFL );	
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

	// kill running opiealarm
	opiealarm_was_running = kill_with_pidfile ( );
	remove_pidfile ( );

	switch ( mode ) {
		case 'r': return resume ( ac_resusp );
		case 's':
		default : return suspend ( fix_rtc );
	}
	return 0;
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
		
	log = fopen ( "/tmp/opiealarm.log", "w" ); 
	log_msg ( "STARTING\n" );
	


	if (!( fp = fopen ( "/etc/resumeat", "r" ))) 
		error_msg_and_die ( 1, "/etc/resumeat" );
	
	if ( !fgets ( buf, sizeof( buf ) - 1, fp ))
		error_msg_and_die ( 1, "/etc/resumeat" );

	fclose ( fp );
	
	alrt = atoi ( buf );
	
	if ( alrt == 0 )
		error_msg_and_die ( 0, "/etc/resumeat contains an invalid time description" );
	
	/* subtract 5 sec from event time... */
	alrt -= 5;

	if ( log )	
		fprintf ( log, "Setting RTC alarm to %d\n", alrt );

	tzset ( );
	
	alr = *gmtime ( &alrt );

	// get system time
	time ( &syst );
	sys = *localtime ( &syst );		
		
	// Write alarm time to RTC
	if (( fd = open ( "/dev/misc/rtc", O_RDWR )) < 0 )
		error_msg_and_die ( 1, "/dev/misc/rtc" );	
		
	// get RTC time
	memset ( &rtc, 0, sizeof ( struct tm ));
	if ( ioctl ( fd, RTC_ALM_SET, &rtc ) < 0 )
		error_msg_and_die ( 1, "ioctl RTC_RD_TIME" );		
	rtct = mktime ( &rtc );

	fprintf ( log, "System  time: %02d.%02d.%04d %02d:%02d:%02d DST: %d (TZ: %s, offset: %d)\n", sys. tm_mday, sys. tm_mon + 1, sys. tm_year + 1900, sys. tm_hour, sys. tm_min, sys. tm_sec, sys. tm_isdst, sys. tm_zone, sys. tm_gmtoff );
	fprintf ( log, "RTC     time: %02d.%02d.%04d %02d:%02d:%02d DST: %d (TZ: %s, offset: %d)\n", rtc. tm_mday, rtc. tm_mon + 1, rtc. tm_year + 1900, rtc. tm_hour, rtc. tm_min, rtc. tm_sec, rtc. tm_isdst, rtc. tm_zone, rtc. tm_gmtoff );
	fprintf ( log, "Wakeup  time: %02d.%02d.%04d %02d:%02d:%02d DST: %d (TZ: %s, offset: %d)\n", alr. tm_mday, alr. tm_mon + 1, alr. tm_year + 1900, alr. tm_hour, alr. tm_min, alr. tm_sec, alr. tm_isdst, alr. tm_zone, alr. tm_gmtoff );

	fprintf ( log, "System/RTC diff: %d seconds\n", (  syst - rtct ) - sys. tm_gmtoff );


	rtc_sys_diff = ( syst - rtct ) - sys. tm_gmtoff;
	
	if ( fix_rtc && (( rtc_sys_diff < -4 ) || ( rtc_sys_diff > 4 ))) {
		struct tm set;
		
		set = *gmtime ( &syst );
	
		fprintf ( log, "Correcting RTC: %d seconds\n", rtc_sys_diff );

	    if ( ioctl ( fd, RTC_SET_TIME, &set ) < 0 )
	       error_msg_and_die ( 1, "ioctl RTC_SET_TIME" );
	}

	// set alarm time
	if ( ioctl ( fd, RTC_ALM_SET, &alr ) < 0 )
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
	
	log_msg ( "EXITING\n" );
	
	fclose ( log );
	remove_pidfile ( );
	
	return 0;
}


int onac ( void )
{
	FILE *fp;
	int on = 0;

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

	if ( !opiealarm_was_running ) { // opiealarm -s got it's RTC signal -> wake up by RTC
		if ( resuspend && onac ( )) {	
			time_t start, now;
			char *argv [4];
						
			if ( !fork_with_pidfile ( ))
				return 4;

			// sleep <resuspend> sec (not less!)
			time ( &start );
			do {
				sleep ( 1 );
				time ( &now );
			} while (( now - start ) < resuspend );

			if ( onac ( )) { // still on ac
				// system() without fork
				argv[0] = "qcop";
				argv[1] = "QPE/Desktop";
				argv[2] = "suspend()";
				argv[3] = 0;
	
				// hard coded for now ...but needed
				setenv ( "LOGNAME", "root", 1 );
				setenv ( "HOME", "/root", 1 );
				setenv ( "LD_LIBRARY_PATH", "/opt/QtPalmtop/lib", 1 );
				setenv ( "QTDIR", "/opt/QtPalmtop", 1 );
			
				remove_pidfile ( );
			
				execv ( "/opt/QtPalmtop/bin/qcop", argv );
				
				perror ( "exec for qcop failed" );
				return 5;
			}
		}
	}
	return 0;
}
