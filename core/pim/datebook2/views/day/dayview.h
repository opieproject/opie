#ifndef OPIE_DATEBOOK_DAYVIEW_H
#define OPIE_DATEBOOK_DAYVIEW_H

#include <qpixmap.h>
#include <qdatetime.h>
#include <qwidget.h>
#include <qvbox.h>

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>
#include <opie2/odatebookaccess.h>

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

        virtual QPixmap pixmap()const;
        virtual QCString type()const;
        virtual QString name()const;
        virtual QString description()const;
        virtual int currentItem()const;

        virtual void currentRange( const QDateTime& src, const QDateTime& from);

        virtual void clockChanged();
        virtual void dayChanged();
        virtual void showDay( const QDate& date );
        virtual QWidget* widget();
        virtual void reschedule();

        void setSelectedWidget( DateBookDayWidget * );
        DateBookDayWidget * getSelectedWidget( void );
        void setJumpToCurTime( bool bJump );
        void setRowStyle( int style );
        QDate date() const;
        DateBookDayView *dayViewWidget();

    public slots:
        void updateView();  //updates TimeMarker and DayWidget-colors
        void dateChanged( int y, int m, int d );

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
        DateBookDayWidget *selectedWidget; //actual selected widget (obviously)
        DateBookDayTimeMarker *timeMarker;  //marker for current time

        virtual void doLoadConfig( Config* );
        virtual void doSaveConfig( Config* );

        void initUI(QWidget *parent);

    private slots:
        void slotColWidthChanged() { relayoutPage(); };

    private:
        DateBookDayWidget *intersects( const DateBookDayWidget *item, const QRect &geom );
        void relayoutPage( bool fromResize = false );
        void getEvents();
    };
}
}

#endif
