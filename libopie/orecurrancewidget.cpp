#include <qapplication.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qspinbox.h>

#include <qpe/timestring.h>

#include "orecurrancewidget.h"

// Global Templates for use in setting up the repeat label...
const QString strDayTemplate = QObject::tr("Every");
const QString strYearTemplate = QObject::tr("%1 %2 every ");
const QString strMonthDateTemplate = QObject::tr("The %1 every ");
const QString strMonthDayTemplate = QObject::tr("The %1 %1 of every");
const QString strWeekTemplate = QObject::tr("Every ");
const QString dayLabel[] = { QObject::tr("Monday"),
                             QObject::tr("Tuesday"),
			     QObject::tr("Wednesday"),
			     QObject::tr("Thursday"),
			     QObject::tr("Friday"),
			     QObject::tr("Saturday"),
			     QObject::tr("Sunday") };


static QString numberPlacing( int x );	// return the proper word format for
                                        // x (1st, 2nd, etc)
static int week( const QDate &dt );    // what week in the month is dt?


ORecurranceWidget::ORecurranceWidget( bool startOnMonday,
                                      const QDate& newStart,
                                      QWidget* parent,
                                      const char* name,
                                      bool modal,
                                      WFlags fl )
    : ORecurranceBase( parent, name, modal, fl ),
      start( newStart ),
      currInterval( None ),
      startWeekOnMonday( startOnMonday )
{
    init();
    fraType->setButton( currInterval );
    chkNoEnd->setChecked( TRUE );
    setupNone();
}
ORecurranceWidget::ORecurranceWidget( bool startOnMonday,
                                      const ORecur& rp, const QDate& startDate,
                                      QWidget* parent, const char* name,
                                      bool modal, WFlags fl)
    : ORecurranceBase( parent, name, modal, fl ),
      start( startDate ),
      end( rp.endDate() ),
      startWeekOnMonday( startOnMonday )
{
    // do some stuff with the repeat pattern
    init();
    switch ( rp.type() ) {
    default:
    case ORecur::NoRepeat:
        currInterval = None;
        setupNone();
        break;
    case ORecur::Daily:
        currInterval = Day;
        setupDaily();
        break;
    case ORecur::Weekly:
        currInterval = Week;
        setupWeekly();
        int day, buttons;
        for ( day = 0x01, buttons = 0; buttons < 7;
              day = day << 1, buttons++ ) {
            if ( rp.days() & day ) {
                if ( startWeekOnMonday )
                    fraExtra->setButton( buttons );
                else {
                    if ( buttons == 7 )
                        fraExtra->setButton( 0 );
                    else
                        fraExtra->setButton( buttons + 1 );
 		    }
		}
	    }
        slotWeekLabel();
        break;
    case ORecur::MonthlyDay:
        currInterval = Month;
        setupMonthly();
        fraExtra->setButton( 0 );
        slotMonthLabel( 0 );
        break;
    case ORecur::MonthlyDate:
        currInterval = Month;
        setupMonthly();
        fraExtra->setButton( 1 );
        slotMonthLabel( 1 );
        break;
    case ORecur::Yearly:
        currInterval = Year;
        setupYearly();
        break;
    }
    fraType->setButton( currInterval );
    spinFreq->setValue( rp.frequency() );
    if ( !rp.hasEndDate() ) {
	cmdEnd->setText( tr("No End Date") );
	chkNoEnd->setChecked( TRUE );
    } else
	cmdEnd->setText( TimeString::shortDate( end ) );
}
ORecurranceWidget::~ORecurranceWidget() {

}
ORecur ORecurranceWidget::recurrance()const {
    QListIterator<QToolButton> it( listRTypeButtons );
    QListIterator<QToolButton> itExtra( listExtra );
    ORecur rpTmp;
    int i;
    for ( i = 0; *it; ++it, i++ ) {
	if ( (*it)->isOn() ) {
	    switch ( i ) {
            case None:
                rpTmp.setType( ORecur::NoRepeat );
                break;
            case Day:
                rpTmp.setType( ORecur::Daily );
                break;
            case Week:{
                rpTmp.setType( ORecur::Weekly );
                int day;
                int day2 = 0;
                for ( day = 1; *itExtra; ++itExtra, day = day << 1 ) {
                    if ( (*itExtra)->isOn() ) {
                        if ( startWeekOnMonday )
                            day2 |= day;
                        else {
                            if ( day == 1 )
                                day2 |= Event::SUN;
                            else
                                day2 |= day >> 1;
                        }
                    }
                }
                rpTmp.setDays( day2 );
            }
                break;
            case Month:
                if ( cmdExtra1->isOn() )
                    rpTmp.setType( ORecur::MonthlyDay );
                else if ( cmdExtra2->isOn() )
                    rpTmp.setType( ORecur::MonthlyDate );
                // figure out the montly day...
                rpTmp.setPosition(  week( start ) );
                break;
            case Year:
                rpTmp.setType( ORecur::Yearly );
                break;
	    }
	    break;  // no need to keep looking!
	}
    }
    rpTmp.setFrequency(spinFreq->value() );
    rpTmp.setHasEndDate( !chkNoEnd->isChecked() );
    if ( rpTmp.hasEndDate() ) {
	rpTmp.setEndDate( end );
    }
    // timestamp it...
    rpTmp.setCreateTime( time( NULL ) );
    return rpTmp;
}
QDate ORecurranceWidget::endDate()const {
    return end;
}
void ORecurranceWidget::slotSetRType(int rtype) {
   // now call the right function based on the type...
    currInterval = static_cast<repeatButtons>(rtype);
    switch ( currInterval ) {
    case None:
        setupNone();
        break;
    case Day:
        setupDaily();
        break;
    case Week:
        setupWeekly();
        slotWeekLabel();
        break;
    case Month:
        setupMonthly();
        cmdExtra2->setOn( TRUE );
        slotMonthLabel( 1 );
        break;
    case Year:
        setupYearly();
        break;
    }
}
void ORecurranceWidget::endDateChanged(int y, int m, int d) {
    end.setYMD( y, m, d );
    if ( end < start )
	end = start;
    cmdEnd->setText(  TimeString::shortDate( end ) );
    repeatPicker->setDate( end.year(), end.month(), end.day() );
}
void ORecurranceWidget::slotNoEnd( bool unused) {
    // if the item was toggled, then go ahead and set it to the maximum date
    if ( unused ) {
	end.setYMD( 3000, 12, 31 );
	cmdEnd->setText( tr("No End Date") );
    } else {
	end = start;
	cmdEnd->setText( TimeString::shortDate(end) );
    }
}
void ORecurranceWidget::setupRepeatLabel( const QString& s) {
    lblVar1->setText( s );
}
void ORecurranceWidget::setupRepeatLabel( int x) {
    // change the spelling based on the value of x
    QString strVar2;

    if ( x > 1 )
	lblVar1->show();
    else
	lblVar1->hide();

    switch ( currInterval ) {
	case None:
	    break;
	case Day:
	    if ( x > 1 )
		strVar2 = tr( "days" );
	    else
		strVar2 = tr( "day" );
	    break;
	case Week:
	    if ( x > 1 )
		strVar2 = tr( "weeks" );
	    else
		strVar2 = tr( "week" );
	    break;
	case Month:
	    if ( x > 1 )
		strVar2 = tr( "months" );
	    else
		strVar2 = tr( "month" );
	    break;
	case Year:
	    if ( x > 1 )
		strVar2 = tr( "years" );
	    else
		strVar2 = tr( "year" );
	    break;
    }
    if ( !strVar2.isNull() )
	lblVar2->setText( strVar2 );
}
void ORecurranceWidget::slotWeekLabel() {
    QString str;
    QListIterator<QToolButton> it( listExtra );
    unsigned int i;
    unsigned int keepMe;
    bool bNeedCarriage = FALSE;
    // don't do something we'll regret!!!
    if ( currInterval != Week )
	return;

    if ( startWeekOnMonday )
	keepMe = start.dayOfWeek() - 1;
    else
	keepMe = start.dayOfWeek() % 7;

    QStringList list;
    for ( i = 0; *it; ++it, i++ ) {
	// a crazy check, if you are repeating weekly, the current day
	// must be selected!!!
	if ( i == keepMe && !( (*it)->isOn() ) )
	    (*it)->setOn( TRUE );
	if ( (*it)->isOn() ) {
	    if ( startWeekOnMonday )
		list.append( dayLabel[i] );
	    else {
		if ( i == 0 )
		    list.append( dayLabel[6] );
		else
		    list.append( dayLabel[i - 1] );
	    }
	}
    }
    QStringList::Iterator itStr;
    for ( i = 0, itStr = list.begin(); itStr != list.end(); ++itStr, i++ ) {
	if ( i == 3 )
	    bNeedCarriage = TRUE;
	else
	    bNeedCarriage = FALSE;
	if ( str.isNull() )
	    str = *itStr;
	else if ( i == list.count() - 1 ) {
	    if ( i < 2 )
		str += tr(" and ") + *itStr;
	    else {
		if ( bNeedCarriage )
		    str += tr( ",\nand " ) + *itStr;
		else
		    str += tr( ", and " ) + *itStr;
	    }
	} else {
	    if ( bNeedCarriage )
		str += ",\n" + *itStr;
	    else
		str += ", " + *itStr;
	}
    }
    str = str.prepend( "on " );
    lblWeekVar->setText( str );
}
void ORecurranceWidget::slotMonthLabel(int type) {
    QString str;
    if ( currInterval != Month || type > 1 )
	return;
    if ( type == 1 )
	str = strMonthDateTemplate.arg( numberPlacing(start.day()) );
    else
	str = strMonthDayTemplate.arg( numberPlacing(week(start)))
	      .arg( dayLabel[start.dayOfWeek() - 1] );
    lblRepeat->setText( str );
}
void ORecurranceWidget::slotChangeStartOfWeek( bool onMonday ) {
 startWeekOnMonday = onMonday;
    // we need to make this unintrusive as possible...
    int saveSpin = spinFreq->value();
    char days = 0;
    int day;
    QListIterator<QToolButton> itExtra( listExtra );
    for ( day = 1; *itExtra; ++itExtra, day = day << 1 ) {
	if ( (*itExtra)->isOn() ) {
	    if ( !startWeekOnMonday )
		days |= day;
	    else {
		if ( day == 1 )
		    days |= ORecur::SUN;
		else
		    days |= day >> 1;
	    }
	}
    }
    setupWeekly();
    spinFreq->setValue( saveSpin );
    int buttons;
    for ( day = 0x01, buttons = 0; buttons < 7;
	  day = day << 1, buttons++ ) {
	if ( days & day ) {
	    if ( startWeekOnMonday )
		fraExtra->setButton( buttons );
	    else {
		if ( buttons == 7 )
		    fraExtra->setButton( 0 );
		else
		    fraExtra->setButton( buttons + 1 );
	    }
	}
    }
    slotWeekLabel();
}
void ORecurranceWidget::setupNone() {
   lblRepeat->setText( tr("No Repeat") );
    lblVar1->hide();
    lblVar2->hide();
    hideExtras();
    cmdEnd->hide();
    lblFreq->hide();
    lblEvery->hide();
    lblFreq->hide();
    spinFreq->hide();
    lblEnd->hide();
    lblWeekVar->hide();
}
void ORecurranceWidget::setupDaily() {
  hideExtras();
    lblWeekVar->hide();
    spinFreq->setValue( 1 );
    lblFreq->setText( tr("day(s)") );
    lblVar2->show();
    showRepeatStuff();
    lblRepeat->setText( strDayTemplate );
    setupRepeatLabel( 1 );
}
void ORecurranceWidget::setupWeekly() {
// reshow the buttons...
    fraExtra->setTitle( tr("Repeat On") );
    fraExtra->setExclusive( FALSE );
    fraExtra->show();
    if ( startWeekOnMonday ) {
	cmdExtra1->setText( tr("Mon") );
	cmdExtra2->setText( tr("Tue") );
	cmdExtra3->setText( tr("Wed") );
	cmdExtra4->setText( tr("Thu") );
	cmdExtra5->setText( tr("Fri") );
	cmdExtra6->setText( tr("Sat") );
	cmdExtra7->setText( tr("Sun") );
    } else {
	cmdExtra1->setText( tr("Sun") );
	cmdExtra2->setText( tr("Mon") );
	cmdExtra3->setText( tr("Tue") );
	cmdExtra4->setText( tr("Wed") );
	cmdExtra5->setText( tr("Thu") );
	cmdExtra6->setText( tr("Fri") );
	cmdExtra7->setText( tr("Sat") );
    }
    // I hope clustering these improve performance....
    cmdExtra1->setOn( FALSE );
    cmdExtra2->setOn( FALSE );
    cmdExtra3->setOn( FALSE );
    cmdExtra4->setOn( FALSE );
    cmdExtra5->setOn( FALSE );
    cmdExtra6->setOn( FALSE );
    cmdExtra7->setOn( FALSE );

    cmdExtra1->show();
    cmdExtra2->show();
    cmdExtra3->show();
    cmdExtra4->show();
    cmdExtra5->show();
    cmdExtra6->show();
    cmdExtra7->show();

    lblWeekVar->show();
    spinFreq->setValue( 1 );
    // might as well set the day too...
    if ( startWeekOnMonday ) {
	fraExtra->setButton( start.dayOfWeek() - 1 );
    } else {
	fraExtra->setButton( start.dayOfWeek() % 7 );
    }
    lblFreq->setText( tr("week(s)") );
    lblVar2->show();
    showRepeatStuff();
    setupRepeatLabel( 1 );
}
void ORecurranceWidget::setupMonthly() {
    hideExtras();
    lblWeekVar->hide();
    fraExtra->setTitle( tr("Repeat By") );
    fraExtra->setExclusive( TRUE );
    fraExtra->show();
    cmdExtra1->setText( tr("Day") );
    cmdExtra1->show();
    cmdExtra2->setText( tr("Date") );
    cmdExtra2->show();
    spinFreq->setValue( 1 );
    lblFreq->setText( tr("month(s)") );
    lblVar2->show();
    showRepeatStuff();
    setupRepeatLabel( 1 );
}
void ORecurranceWidget::setupYearly() {
hideExtras();
    lblWeekVar->hide();
    spinFreq->setValue( 1 );
    lblFreq->setText( tr("year(s)") );
    lblFreq->show();
    lblFreq->show();
    showRepeatStuff();
    lblVar2->show();
    QString strEvery = strYearTemplate.arg( start.monthName(start.month()) ).arg( numberPlacing(start.day()) );
    lblRepeat->setText( strEvery );
    setupRepeatLabel( 1 );

}
void ORecurranceWidget::init() {
 QPopupMenu *m1 = new QPopupMenu( this );
    repeatPicker = new DateBookMonth( m1, 0, TRUE );
    m1->insertItem( repeatPicker );
    cmdEnd->setPopup( m1 );
    cmdEnd->setPopupDelay( 0 );

    QObject::connect( repeatPicker, SIGNAL(dateClicked(int, int, int)),
                      this, SLOT(endDateChanged(int, int, int)) );
    QObject::connect( qApp, SIGNAL(weekChanged(bool)),
		      this, SLOT(slotChangeStartOfWeek(bool)) );

    listRTypeButtons.setAutoDelete( TRUE );
    listRTypeButtons.append( cmdNone );
    listRTypeButtons.append( cmdDay );
    listRTypeButtons.append( cmdWeek );
    listRTypeButtons.append( cmdMonth );
    listRTypeButtons.append( cmdYear );

    listExtra.setAutoDelete( TRUE );
    listExtra.append( cmdExtra1 );
    listExtra.append( cmdExtra2 );
    listExtra.append( cmdExtra3 );
    listExtra.append( cmdExtra4 );
    listExtra.append( cmdExtra5 );
    listExtra.append( cmdExtra6 );
    listExtra.append( cmdExtra7 );
}
void ORecurranceWidget::hideExtras() {
      // hide the extra buttons...
    fraExtra->hide();
    chkNoEnd->hide();
    QListIterator<QToolButton> it( listExtra );
    for ( ; *it; ++it ) {
	(*it)->hide();
	(*it)->setOn( FALSE );
    }
}
void ORecurranceWidget::showRepeatStuff() {
    cmdEnd->show();
    chkNoEnd->show();
    lblFreq->show();
    lblEvery->show();
    lblFreq->show();
    spinFreq->show();
    lblEnd->show();
    lblRepeat->setText( tr("Every") );
}


static int week( const QDate &start )
{
    // figure out the week...
    int stop = start.day(),
        sentinel = start.dayOfWeek(),
        dayOfWeek = QDate( start.year(), start.month(), 1 ).dayOfWeek(),
        week = 1,
	i;
    for ( i = 1; i < stop; i++ ) {
	if ( dayOfWeek++ == sentinel )
	    week++;
	if ( dayOfWeek > 7 )
	    dayOfWeek = 0;
    }
    return week;
}

static QString numberPlacing( int x )
{
    // I hope this works in other languages besides english...
    QString str = QString::number( x );
    switch ( x % 10 ) {
	case 1:
	    str += QWidget::tr( "st" );
	    break;
	case 2:
	    str += QWidget::tr( "nd" );
	    break;
	case 3:
	    str += QWidget::tr( "rd" );
	    break;
	default:
	    str += QWidget::tr( "th" );
	    break;
    }
    return str;
}
