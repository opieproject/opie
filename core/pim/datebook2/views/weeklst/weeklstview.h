#ifndef OPIE_DATEBOOK_WEEKLSTVIEW_H
#define OPIE_DATEBOOK_WEEKLSTVIEW_H

#include <qpixmap.h>
#include <qdatetime.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>

#include "view.h"

class DateBookWeekLstHeader;
class DateBookWeekLstDblView;
class Config;

namespace Opie {
namespace Datebook {
    class MainWindow;
    class WeekLstView : public View {
        Q_OBJECT
    public:
        WeekLstView( MainWindow* window,  QWidget* parent );

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

        void popup( const OPimOccurrence &occ, const QPoint &pt );
        QDate weekDate() const;
        void setDate( int y, int m, int d );
        void setDate( const QDate &d );

    protected:
        QWidget *m_widget;
        bool m_dbl;
        QDate bdate;
        int year, _week, dow;
        DateBookWeekLstHeader *m_header;
        QVBoxLayout *m_layout;
        QScrollView *m_scroll;
        DateBookWeekLstDblView *m_CurrentView;
        bool dateset:1;
        OPimOccurrence popupItem;
        int m_timeDisplay;

        virtual void doLoadConfig( Config* );
        virtual void doSaveConfig( Config* );

        void initUI(QWidget *parent);
        void getEvents();

    protected slots:
        void dateChanged( QDate &newdate );
        void setDbl(bool on);
        void showDate(int,int,int);
        void addEvent( const QDateTime &, const QDateTime & );

    private:
    };
}
}

#endif
