#ifndef OPIE_DATEBOOK_DAYVIEW_H
#define OPIE_DATEBOOK_DAYVIEW_H

#include <qpixmap.h>
#include <qdatetime.h>
#include <qwidget.h>
#include <qvbox.h>

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>

#include "view.h"

class Config;
class DateBookDayView;
class DateBookDayHeader;
class DatebookdayAllday;
class DateBookDayTimeMarker;
class DateBookDayWidget;
class WidgetListClass;

namespace Opie {
namespace Datebook {
    class MainWindow;
    class DayView : public View {
        Q_OBJECT
    public:
        DayView( MainWindow* window,  QWidget* parent );
        virtual ~DayView();

        virtual QPixmap pixmap() const;
        virtual QCString type() const;
        virtual QString name() const;
        virtual QString description() const;
        virtual const OPimOccurrence *currentItem() const;
        virtual QDate date() const;

        virtual void currentRange( const QDateTime& src, const QDateTime& from);

        virtual void clockChanged();
        virtual void dayChanged();
        virtual void timeChanged();
        virtual void showDay( const QDate& date );
        virtual QWidget* widget();
        virtual void reschedule();

        virtual QWidget *createConfigWidget( QWidget *owner );
        virtual void readConfigWidget( QWidget *widget );

        void popup( const QPoint &pt );

        void setSelectedWidget( DateBookDayWidget * );
        DateBookDayWidget * getSelectedWidget( void );
        void setJumpToCurTime( bool bJump );
        void setRowStyle( int style );
        DateBookDayView *dayViewWidget();

    public slots:
        void updateView();  //updates TimeMarker and DayWidget-colors
        void dateChanged( int y, int m, int d );
        void slotViewDateChanged( int y, int m, int d );
        void insertEvent(const Opie::OPimEvent &);

    protected:
        QVBox *m_box;
        QDate currDate;
        DateBookDayView *m_view;
        DateBookDayHeader *m_header;
        DatebookdayAllday *m_allDays;
        WidgetListClass *widgetList; //reimplemented QList for sorting widgets by height
        int startTime;
        bool jumpToCurTime; //should we jump to current time in dayview?
        int rowStyle;
        bool m_disabled;
        DateBookDayWidget *selectedWidget; //actual selected widget (obviously)
        DateBookDayTimeMarker *timeMarker;  //marker for current time

        virtual void doLoadConfig( Config* );
        virtual void doSaveConfig( Config* );

        void initUI(QWidget *parent);

    private slots:
        void slotColWidthChanged() { relayoutPage(); };
        void slotNewEvent( const QString & );

    private:
        DateBookDayWidget *intersects( const DateBookDayWidget *item, const QRect &geom );
        void relayoutPage( bool fromResize = false );
        void getEvents();
    };
}
}

#endif
