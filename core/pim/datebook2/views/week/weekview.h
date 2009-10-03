#ifndef OPIE_DATEBOOK_WEEKVIEW_H
#define OPIE_DATEBOOK_WEEKVIEW_H

#include <qpixmap.h>
#include <qdatetime.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlabel.h>

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>

#include "view.h"

class Config;
class DateBookWeekView;
class DateBookWeekHeader;
class DatebookWeekItemPreview;

namespace Opie {
namespace Datebook {
    class MainWindow;
    class WeekView : public View {
        Q_OBJECT
    public:
        WeekView( MainWindow* window,  QWidget* parent );

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

        DateBookWeekView *weekViewWidget();
        QDate weekDate() const;
        void setDate( int y, int m, int d );
        void setDate( const QDate &d );

    protected:
        QWidget *m_widget;
        int year;
        int _week;
        int dow;
        QDate bdate;
        DateBookWeekHeader *m_header;
        DateBookWeekView *m_view;
        DatebookWeekItemPreview *m_preview;
        QTimer *tHide;
        int startTime;

        virtual void doLoadConfig( Config* );
        virtual void doSaveConfig( Config* );

        void initUI(QWidget *parent);
        void getEvents();

    private slots:
        void showDay( int day );
        void dateChanged( QDate &newdate );
        void slotShowEvent( const Opie::OPimOccurrence & );
        void slotHideEvent();
        void slotYearChanged( int );
        void slotViewDateChanged( QDate &newdate );

    private:
        void generateAllDayTooltext( QString& text );
        void generateNormalTooltext( QString& text,
                                 const Opie::OPimOccurrence &ev);
    };
}
}

#endif
