#include <qregexp.h>

#include "kdebug.h"

#include "klocale.h"

QString i18n(const char *text)
{
  return QString( text );
}

QString i18n(const char *,const char *text)
{
  return QString( text );
}

inline void put_it_in( QChar *buffer, uint& index, const QString &s )
{
  for ( uint l = 0; l < s.length(); l++ )
    buffer[index++] = s.at( l );
}

inline void put_it_in( QChar *buffer, uint& index, int number )
{
  buffer[index++] = number / 10 + '0';
  buffer[index++] = number % 10 + '0';
}

static int readInt(const QString &str, uint &pos)
{
  if (!str.at(pos).isDigit()) return -1;
  int result = 0;
  for (; str.length() > pos && str.at(pos).isDigit(); pos++)
    {
      result *= 10;
      result += str.at(pos).digitValue();
    }

  return result;
}

QString KLocale::formatTime(const QTime &pTime, bool includeSecs) const
{
  const QString rst = timeFormat();

  // only "pm/am" here can grow, the rest shrinks, but
  // I'm rather safe than sorry
  QChar *buffer = new QChar[rst.length() * 3 / 2 + 30];

  uint index = 0;
  bool escape = false;
  int number = 0;

  for ( uint format_index = 0; format_index < rst.length(); format_index++ )
    {
      if ( !escape )
	{
	  if ( rst.at( format_index ).unicode() == '%' )
	    escape = true;
	  else
	    buffer[index++] = rst.at( format_index );
	}
      else
	{
	  switch ( rst.at( format_index ).unicode() )
	    {
	    case '%':
	      buffer[index++] = '%';
	      break;
	    case 'H':
	      put_it_in( buffer, index, pTime.hour() );
	      break;
	    case 'I':
	      put_it_in( buffer, index, ( pTime.hour() + 11) % 12 + 1 );
	      break;
	    case 'M':
	      put_it_in( buffer, index, pTime.minute() );
	      break;
	    case 'S':
	      if (includeSecs)
		put_it_in( buffer, index, pTime.second() );
	      else
		{
		  // we remove the seperator sign before the seconds and
		  // assume that works everywhere
		  --index;
		  break;
		}
	      break;
	    case 'k':
	      number = pTime.hour();
	    case 'l':
	      // to share the code
	      if ( rst.at( format_index ).unicode() == 'l' )
		number = (pTime.hour() + 11) % 12 + 1;
	      if ( number / 10 )
		buffer[index++] = number / 10 + '0';
	      buffer[index++] = number % 10 + '0';
	      break;
	    case 'p':
	      {
		QString s;
		if ( pTime.hour() >= 12 )
		  put_it_in( buffer, index, i18n("pm") );
		else
		  put_it_in( buffer, index, i18n("am") );
		break;
	      }
	    default:
	      buffer[index++] = rst.at( format_index );
	      break;
	    }
	  escape = false;
	}
    }
  QString ret( buffer, index );
  delete [] buffer;
  return ret;
}    

QString KLocale::formatDate(const QDate &pDate, bool shortFormat) const
{
  const QString rst = shortFormat?dateFormatShort():dateFormat();

  // I'm rather safe than sorry
  QChar *buffer = new QChar[rst.length() * 3 / 2 + 50];

  unsigned int index = 0;
  bool escape = false;
  int number = 0;

  for ( uint format_index = 0; format_index < rst.length(); ++format_index )
    {
      if ( !escape )
	{
	  if ( rst.at( format_index ).unicode() == '%' )
	    escape = true;
	  else
	    buffer[index++] = rst.at( format_index );
	}
      else
	{
	  switch ( rst.at( format_index ).unicode() )
	    {
	    case '%':
	      buffer[index++] = '%';
	      break;
	    case 'Y':
	      put_it_in( buffer, index, pDate.year() / 100 );
	    case 'y':
	      put_it_in( buffer, index, pDate.year() % 100 );
	      break;
	    case 'n':
	      number = pDate.month();
	    case 'e':
	      // to share the code
	      if ( rst.at( format_index ).unicode() == 'e' )
		number = pDate.day();
	      if ( number / 10 )
		buffer[index++] = number / 10 + '0';
	      buffer[index++] = number % 10 + '0';
	      break;
	    case 'm':
	      put_it_in( buffer, index, pDate.month() );
	      break;
	    case 'b':
	      put_it_in( buffer, index, monthName(pDate.month(), true) );
	      break;
	    case 'B':
	      put_it_in( buffer, index, monthName(pDate.month(), false) );
	      break;
	    case 'd':
	      put_it_in( buffer, index, pDate.day() );
	      break;
	    case 'a':
	      put_it_in( buffer, index, weekDayName(pDate.dayOfWeek(), true) );
	      break;
	    case 'A':
	      put_it_in( buffer, index, weekDayName(pDate.dayOfWeek(), false) );
	      break;
	    default:
	      buffer[index++] = rst.at( format_index );
	      break;
	    }
	  escape = false;
	}
    }
  QString ret( buffer, index );
  delete [] buffer;
  return ret;
}

QString KLocale::formatDateTime(const QDateTime &pDateTime,
				bool shortFormat,
				bool includeSeconds) const
{
  return i18n("concatenation of dates and time", "%1 %2")
    .arg( formatDate( pDateTime.date(), shortFormat ) )
    .arg( formatTime( pDateTime.time(), includeSeconds ) );
}

QString KLocale::formatDateTime(const QDateTime &pDateTime) const
{
  return formatDateTime(pDateTime, true);
}

QDate KLocale::readDate(const QString &intstr, bool* ok) const
{
  QDate date;
  date = readDate(intstr, true, ok);
  if (date.isValid()) return date;
  return readDate(intstr, false, ok);
}

QDate KLocale::readDate(const QString &intstr, bool shortFormat, bool* ok) const
{
  QString fmt = (shortFormat ? dateFormatShort() : dateFormat()).simplifyWhiteSpace();
  return readDate( intstr, fmt, ok );
}

QDate KLocale::readDate(const QString &intstr, const QString &fmt, bool* ok) const
{
  //kdDebug(173) << "KLocale::readDate intstr=" << intstr << " fmt=" << fmt << endl;
  QString str = intstr.simplifyWhiteSpace().lower();
  int day = -1, month = -1;
  // allow the year to be omitted if not in the format
  int year = QDate::currentDate().year();
  uint strpos = 0;
  uint fmtpos = 0;

  while (fmt.length() > fmtpos || str.length() > strpos)
    {
      if ( !(fmt.length() > fmtpos && str.length() > strpos) )
        goto error;

      QChar c = fmt.at(fmtpos++);

      if (c != '%') {
        if (c.isSpace())
          strpos++;
        else if (c != str.at(strpos++))
          goto error;
        continue;
      }

      // remove space at the begining
      if (str.length() > strpos && str.at(strpos).isSpace())
        strpos++;

      c = fmt.at(fmtpos++);
      switch (c)
      {
	case 'a':
	case 'A':
	  // this will just be ignored
	  { // Cristian Tache: porting to Win: Block added because of "j" redefinition
  	  for (int j = 1; j < 8; j++) {
  	    QString s = weekDayName(j, c == 'a').lower();
  	    int len = s.length();
  	    if (str.mid(strpos, len) == s)
  	      strpos += len;
  	  }
  	  break;
  	}
	case 'b':
	case 'B':
	  { // Cristian Tache: porting to Win: Block added because of "j" redefinition
  	  for (int j = 1; j < 13; j++) {
  	    QString s = monthName(j, c == 'b').lower();
  	    int len = s.length();
  	    if (str.mid(strpos, len) == s) {
  	      month = j;
  	      strpos += len;
  	    }
  	  }
  	  break;
  	}
	case 'd':
	case 'e':
	  day = readInt(str, strpos);
	  if (day < 1 || day > 31)
	    goto error;

	  break;

	case 'n':
	case 'm':
	  month = readInt(str, strpos);
	  if (month < 1 || month > 12)
	    goto error;

	  break;

	case 'Y':
	case 'y':
	  year = readInt(str, strpos);
	  if (year < 0)
	    goto error;
	  // Qt treats a year in the range 0-100 as 1900-1999.
	  // It is nicer for the user if we treat 0-68 as 2000-2068
	  if (year < 69)
	    year += 2000;
	  else if (c == 'y')
	    year += 1900;

	  break;
	}
    }
  //kdDebug(173) << "KLocale::readDate day=" << day << " month=" << month << " year=" << year << endl;
  if ( year != -1 && month != -1 && day != -1 )
  {
    if (ok) *ok = true;
    return QDate(year, month, day);
  }
 error:
  if (ok) *ok = false;
  return QDate(); // invalid date
}

QTime KLocale::readTime(const QString &intstr, bool *ok) const
{
  QTime _time;
  _time = readTime(intstr, true, ok);
  if (_time.isValid()) return _time;
  return readTime(intstr, false, ok);
}

QTime KLocale::readTime(const QString &intstr, bool seconds, bool *ok) const
{
  QString str = intstr.simplifyWhiteSpace().lower();
  QString Format = timeFormat().simplifyWhiteSpace();
  if (!seconds)
    Format.replace(QRegExp(QString::fromLatin1(".%S")), QString::null);

  int hour = -1, minute = -1, second = seconds ? -1 : 0; // don't require seconds
  bool g_12h = false;
  bool pm = false;
  uint strpos = 0;
  uint Formatpos = 0;

  while (Format.length() > Formatpos || str.length() > strpos)
    {
      if ( !(Format.length() > Formatpos && str.length() > strpos) ) goto error;

      QChar c = Format.at(Formatpos++);

      if (c != '%')
	{
	  if (c.isSpace())
	    strpos++;
	  else if (c != str.at(strpos++))
	    goto error;
	  continue;
	}

      // remove space at the begining
      if (str.length() > strpos && str.at(strpos).isSpace())
	strpos++;

      c = Format.at(Formatpos++);
      switch (c)
	{
	case 'p':
	  {
	    QString s;
	    s = i18n("pm").lower();
	    int len = s.length();
	    if (str.mid(strpos, len) == s)
	      {
		pm = true;
		strpos += len;
	      }
	    else
	      {
		s = i18n("am").lower();
		len = s.length();
		if (str.mid(strpos, len) == s) {
		  pm = false;
		  strpos += len;
		}
		else
		  goto error;
	      }
	  }
	  break;

	case 'k':
	case 'H':
	  g_12h = false;
	  hour = readInt(str, strpos);
	  if (hour < 0 || hour > 23)
	    goto error;

	  break;

	case 'l':
	case 'I':
	  g_12h = true;
	  hour = readInt(str, strpos);
	  if (hour < 1 || hour > 12)
	    goto error;

	  break;

	case 'M':
	  minute = readInt(str, strpos);
	  if (minute < 0 || minute > 59)
	    goto error;

	  break;

	case 'S':
	  second = readInt(str, strpos);
	  if (second < 0 || second > 59)
	    goto error;

	  break;
	}
    }
  if (g_12h)
    {
      hour %= 12;
      if (pm) hour += 12;
    }

  if (ok) *ok = true;
  return QTime(hour, minute, second);

 error:
  if (ok) *ok = false;
  return QTime(-1, -1, -1); // return invalid date if it didn't work
  // This will be removed in the near future, since it gives a warning on stderr.
  // The presence of the bool* (since KDE-3.0) removes the need for an invalid QTime.
}

bool KLocale::use12Clock() const
{
  return false;
}

bool KLocale::weekStartsMonday() const
{
  return true;
}

QString KLocale::weekDayName(int i,bool shortName) const
{
  if ( shortName )
    switch ( i )
      {
      case 1:  return i18n("Monday", "Mon");
      case 2:  return i18n("Tuesday", "Tue");
      case 3:  return i18n("Wednesday", "Wed");
      case 4:  return i18n("Thursday", "Thu");
      case 5:  return i18n("Friday", "Fri");
      case 6:  return i18n("Saturday", "Sat");
      case 7:  return i18n("Sunday", "Sun");
      }
  else
    switch ( i )
      {
      case 1:  return i18n("Monday");
      case 2:  return i18n("Tuesday");
      case 3:  return i18n("Wednesday");
      case 4:  return i18n("Thursday");
      case 5:  return i18n("Friday");
      case 6:  return i18n("Saturday");
      case 7:  return i18n("Sunday");
      }

  return QString::null;
}

QString KLocale::monthName(int i,bool shortName) const
{
  if ( shortName )
    switch ( i )
      {
      case 1:   return i18n("January", "Jan");
      case 2:   return i18n("February", "Feb");
      case 3:   return i18n("March", "Mar");
      case 4:   return i18n("April", "Apr");
      case 5:   return i18n("May short", "May");
      case 6:   return i18n("June", "Jun");
      case 7:   return i18n("July", "Jul");
      case 8:   return i18n("August", "Aug");
      case 9:   return i18n("September", "Sep");
      case 10:  return i18n("October", "Oct");
      case 11:  return i18n("November", "Nov");
      case 12:  return i18n("December", "Dec");
      }
  else
    switch (i)
      {
      case 1:   return i18n("January");
      case 2:   return i18n("February");
      case 3:   return i18n("March");
      case 4:   return i18n("April");
      case 5:   return i18n("May long", "May");
      case 6:   return i18n("June");
      case 7:   return i18n("July");
      case 8:   return i18n("August");
      case 9:   return i18n("September");
      case 10:  return i18n("October");
      case 11:  return i18n("November");
      case 12:  return i18n("December");
      }

  return QString::null;
}

QString KLocale::country() const
{
  return QString::null;
}

QString KLocale::dateFormat() const
{
  return "%A %d %B %Y";
}

QString KLocale::dateFormatShort() const
{
  return "%d.%m.%Y";
}

QString KLocale::timeFormat() const
{
  return "%H:%M:%S";
}
