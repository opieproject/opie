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


void error_msg_and_die ( int perr, const char *msg )
{
	if ( perr )
		perror ( msg );
	else
		fprintf ( stderr, "%s\n", msg );
	exit ( 1 );
}


void extractevent ( ) 
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
	tm = gmtime ( &t );

	/* Write alarm time to RTC */
	fd = open ( "/dev/misc/rtc", O_RDWR );
	if ( fd < 0 ) 
		error_msg_and_die ( 1, "/dev/misc/rtc" );
	
	// set alarm time
	if ( ioctl ( fd, RTC_ALM_SET, tm ) < 0 )
		error_msg_and_die ( 1, "ioctl RTC_ALM_SET" );
		
	// enable alarm irq	
	if ( ioctl ( fd, RTC_AIE_ON, 0 ) < 0 )
		error_msg_and_die ( 1, "ioctl RTC_AIE_ON" );
			
	// wait for alarm irq
	if ( read ( fd, buf, sizeof( unsigned long )) < 0 )
		error_msg_and_die ( 1, "read rtc alarm" );
	
	// disable alarm irq
	if ( ioctl ( fd, RTC_AIE_OFF, 0 ) < 0 ) 
		error_msg_and_die ( 1, "ioctl RTC_AIE_OFF" );
		
	close ( fd );
}


int main ( ) 
{
	if ( geteuid ( ) != 0 )
		error_msg_and_die ( 0, "You need root priviledges to run opiealarm." );

	extractevent ( );
	return 0;
}
