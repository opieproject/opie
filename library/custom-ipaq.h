/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <qpe/resource.h>
#include <qsound.h>
#include <qpe/sound.h>

#include <qmessagebox.h>

#ifndef QT_NO_SOUND

#define CUSTOM_SOUND_ALARM \
{ \
	int fd; \
	int vol; \
	bool vol_reset = false; \
 \
	if ((( fd = open ( "/dev/sound/mixer", O_RDWR )) >= 0 ) || \
	    (( fd = open ( "/dev/mixer", O_RDWR )) >= 0 )) { \
 \
		if ( ioctl ( fd, MIXER_READ( 0 ), &vol ) >= 0 ) { \
			Config cfg ( "qpe" ); \
			cfg. setGroup ( "Volume" ); \
 \
 			int volalarm = cfg. readNumEntry ( "AlarmPercent", 50 ); \
			if ( volalarm < 0 ) \
				volalarm = 0; \
			else if ( volalarm > 100 ) \
				volalarm = 100; \
			volalarm |= ( volalarm << 8 ); \
			if ( ioctl ( fd, MIXER_WRITE( 0 ), &volalarm ) >= 0 ) \
				vol_reset = true; \
		} \
	} \
 \
    Sound snd ( "alarm" ); \
    snd. play ( ); \
    while ( !snd. isFinished ( )) \
    	qApp-> processEvents ( ); \
 \
	if ( fd >= 0 ) { \
		if ( vol_reset ) \
			::ioctl ( fd, MIXER_WRITE( 0 ), &vol ); \
		::close ( fd ); \
	} \
}

#define CUSTOM_SOUND_KEYCLICK  { QSound::play ( Resource::findSound ( "keysound" )); }
#define CUSTOM_SOUND_TOUCH     { QSound::play ( Resource::findSound ( "screensound" )); }

#else

#define CUSTOM_SOUND_ALARM     { ; }
#define CUSTOM_SOUND_KEYCLICK  { ; }
#define CUSTOM_SOUND_TOUCH     { ; }


#endif

